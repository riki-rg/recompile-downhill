#include <jni.h>
#include <GLES3/gl3.h>

#include <array>
#include <cctype>
#include <cstdint>
#include <string>
#include <unistd.h>
#include <vector>

namespace {
constexpr std::size_t kSectorSize = 2048;
constexpr off_t kPvdOffset = 16 * kSectorSize;

bool readFully(int fd, off_t offset, void* destination, std::size_t size) {
    auto* bytes = static_cast<std::uint8_t*>(destination);
    std::size_t total = 0;
    while (total < size) {
        const ssize_t count = pread(fd, bytes + total, size - total, offset + static_cast<off_t>(total));
        if (count <= 0) return false;
        total += static_cast<std::size_t>(count);
    }
    return true;
}

std::uint32_t readLittleEndian32(const std::uint8_t* value) {
    return static_cast<std::uint32_t>(value[0]) | (static_cast<std::uint32_t>(value[1]) << 8)
        | (static_cast<std::uint32_t>(value[2]) << 16) | (static_cast<std::uint32_t>(value[3]) << 24);
}

std::string cleanText(const std::uint8_t* bytes, std::size_t length) {
    std::string value;
    for (std::size_t index = 0; index < length; ++index) {
        const unsigned char character = bytes[index];
        if (character != ' ') value.push_back(std::isprint(character) ? static_cast<char>(character) : '?');
    }
    return value.empty() ? "UNTITLED" : value;
}

std::string upperText(const std::uint8_t* bytes, std::size_t length) {
    std::string value;
    value.reserve(length);
    for (std::size_t index = 0; index < length; ++index) {
        value.push_back(static_cast<char>(std::toupper(bytes[index])));
    }
    return value;
}

struct FileRecord {
    std::string name;
    std::uint32_t extent;
    std::uint32_t size;
    bool directory;
};

std::vector<FileRecord> readDirectoryEntries(int fd, std::uint32_t sector, std::uint32_t size) {
    std::vector<FileRecord> records;
    if (size == 0 || size > 1024 * 1024) return records;
    std::vector<std::uint8_t> directory(size);
    if (!readFully(fd, static_cast<off_t>(sector) * kSectorSize, directory.data(), directory.size())) return records;
    std::size_t cursor = 0;
    while (cursor < directory.size()) {
        const std::uint8_t length = directory[cursor];
        if (length == 0) { cursor = ((cursor / kSectorSize) + 1) * kSectorSize; continue; }
        if (length < 34 || cursor + length > directory.size()) break;
        const auto* record = directory.data() + cursor;
        const std::uint8_t nameLength = record[32];
        if (33 + nameLength > length) break;
        if (nameLength != 1 || (record[33] != 0 && record[33] != 1)) {
            FileRecord item;
            item.name = cleanText(record + 33, nameLength);
            item.extent = readLittleEndian32(record + 2);
            item.size = readLittleEndian32(record + 10);
            item.directory = (record[25] & 0x02) != 0;
            records.push_back(std::move(item));
        }
        cursor += length;
    }
    return records;
}

struct TreeStats {
    std::uint64_t totalFiles = 0;
    std::uint64_t totalDirectories = 0;
    std::uint64_t totalBytes = 0;
    std::string largestFile;
    std::uint64_t largestSize = 0;
    std::size_t shownLines = 0;
    bool truncated = false;
};

constexpr std::size_t kMaxTreeDepth = 4;
constexpr std::size_t kMaxTreeLines = 96;
constexpr std::size_t kMaxTreeEntries = 512;

void walkTree(int fd, const std::vector<FileRecord>& entries, std::size_t depth, TreeStats& stats, std::string& tree) {
    for (const auto& item : entries) {
        if (stats.totalFiles + stats.totalDirectories >= kMaxTreeEntries) { stats.truncated = true; return; }
        const bool show = stats.shownLines < kMaxTreeLines;
        if (item.directory) {
            ++stats.totalDirectories;
            if (show) {
                for (std::size_t pad = 0; pad < depth; ++pad) tree += "  ";
                tree += "[D] " + item.name + "\n";
                ++stats.shownLines;
            } else stats.truncated = true;
            if (depth + 1 < kMaxTreeDepth) {
                walkTree(fd, readDirectoryEntries(fd, item.extent, item.size), depth + 1, stats, tree);
            } else stats.truncated = true;
        } else {
            ++stats.totalFiles;
            stats.totalBytes += item.size;
            if (item.size > stats.largestSize) { stats.largestSize = item.size; stats.largestFile = item.name; }
            if (show) {
                for (std::size_t pad = 0; pad < depth; ++pad) tree += "  ";
                tree += item.name + "  |  " + std::to_string(item.size) + " B\n";
                ++stats.shownLines;
            } else stats.truncated = true;
        }
    }
}

std::string inspectIso(int fd) {
    std::array<std::uint8_t, kSectorSize> pvd{};
    if (!readFully(fd, kPvdOffset, pvd.data(), pvd.size())) return "Could not read the ISO-9660 primary volume descriptor.";
    if (pvd[0] != 1 || std::string(reinterpret_cast<const char*>(pvd.data() + 1), 5) != "CD001" || pvd[6] != 1) return "This file is not a readable ISO-9660 image.";

    const std::string volume = cleanText(pvd.data() + 40, 32);
    const std::uint8_t recordLength = pvd[156];
    if (recordLength < 34 || 156 + recordLength > pvd.size()) return "ISO-9660 root directory metadata is malformed.";
    const auto* root = pvd.data() + 156;
    const std::uint32_t rootSector = readLittleEndian32(root + 2);
    const std::uint32_t rootSize = readLittleEndian32(root + 10);
    if (rootSize == 0 || rootSize > 8 * 1024 * 1024) return "ISO-9660 root directory size is unsupported.";

    std::vector<FileRecord> rootEntries = readDirectoryEntries(fd, rootSector, rootSize);
    if (rootEntries.empty()) return "Could not read the ISO-9660 root directory.";

    TreeStats stats;
    std::string tree;
    walkTree(fd, rootEntries, 0, stats, tree);

    std::uint32_t systemSector = 0;
    std::uint32_t systemSize = 0;
    bool hasSystemCnf = false;
    for (const auto& item : rootEntries) {
        if (item.name == "SYSTEM.CNF;1" && !item.directory) {
            hasSystemCnf = true;
            systemSector = item.extent;
            systemSize = item.size;
        }
    }

    std::string report = "ISO-9660 volume: " + volume + "\n\nImage tree (bounded depth 4):\n" + tree;
    if (stats.truncated) report += "\n(tree truncated — showing the first 96 lines only)";
    report += "\n\nImage statistics:\n- Files: " + std::to_string(stats.totalFiles)
        + "\n- Directories: " + std::to_string(stats.totalDirectories)
        + "\n- Indexed payload: " + std::to_string(stats.totalBytes) + " B";
    if (!stats.largestFile.empty()) {
        report += "\n- Largest indexed file: " + stats.largestFile + " (" + std::to_string(stats.largestSize) + " B)";
    }

    report += "\n\nCompatibility verdict: ";
    if (!hasSystemCnf) {
        report += "UNSUPPORTED_LAYOUT — the root directory has no readable SYSTEM.CNF record.";
    } else {
        constexpr std::size_t kSystemCnfLimit = 16 * 1024;
        const std::size_t cnfSize = systemSize > kSystemCnfLimit ? kSystemCnfLimit : systemSize;
        if (systemSize == 0 || cnfSize == 0) {
            report += "UNSUPPORTED_LAYOUT — SYSTEM.CNF metadata is malformed.";
        } else {
            std::vector<std::uint8_t> cnf(cnfSize);
            if (!readFully(fd, static_cast<off_t>(systemSector) * kSectorSize, cnf.data(), cnf.size())) {
                report += "UNSUPPORTED_LAYOUT — SYSTEM.CNF could not be read.";
            } else {
                const std::string text = upperText(cnf.data(), cnf.size());
                if (text.find("SLUS_206.47") != std::string::npos) {
                    report += "USA_CANDIDATE — SYSTEM.CNF advertises SLUS_206.47 (Downhill Domination NTSC-U serial). "
                              "Layout verification is still required before this is treated as a supported build.";
                } else {
                    report += "NON_TARGET_PS2_DISC — SYSTEM.CNF does not advertise the Downhill Domination USA serial.";
                }
            }
        }
    }
    return report;
}

int gWidth = 1;
int gHeight = 1;
float gPhase = 0.0f;

void clearPanel(int x, int y, int width, int height, float red, float green, float blue) {
    glScissor(x, y, width, height);
    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_id_dev_rikirg_downhillnative_MainActivity_nativeRuntimeName(JNIEnv* env, jclass) {
    return env->NewStringUTF("C++20 / Android NDK");
}

extern "C" JNIEXPORT jstring JNICALL
Java_id_dev_rikirg_downhillnative_MainActivity_nativeExploreIso(JNIEnv* env, jclass, jint fd) {
    const std::string result = inspectIso(fd);
    return env->NewStringUTF(result.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_id_dev_rikirg_downhillnative_DiagnosticActivity_nativeGlCreate(JNIEnv*, jclass) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.035f, 0.043f, 0.055f, 1.0f);
}

extern "C" JNIEXPORT void JNICALL
Java_id_dev_rikirg_downhillnative_DiagnosticActivity_nativeGlResize(JNIEnv*, jclass, jint width, jint height) {
    gWidth = width > 0 ? width : 1;
    gHeight = height > 0 ? height : 1;
    glViewport(0, 0, gWidth, gHeight);
}

extern "C" JNIEXPORT void JNICALL
Java_id_dev_rikirg_downhillnative_DiagnosticActivity_nativeGlFrame(JNIEnv*, jclass, jfloat steer, jfloat throttle, jfloat brake) {
    // Original diagnostic-only color field; no proprietary data or gameplay is rendered.
    gPhase += 0.012f + throttle * 0.010f - brake * 0.004f;
    if (gPhase > 6.28318f) gPhase -= 6.28318f;
    const float horizon = 0.12f + (steer + 1.0f) * 0.035f;
    const float trail = 0.16f + throttle * 0.18f;
    glScissor(0, 0, gWidth, gHeight);
    glClearColor(horizon, 0.055f + trail, 0.075f + gPhase * 0.008f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Original schematic downhill test course: sky, terrain, trail and touch zones.
    clearPanel(0, gHeight / 2, gWidth, gHeight / 2, 0.08f, 0.16f, 0.22f);
    clearPanel(0, 0, gWidth, gHeight / 2, 0.055f, 0.11f, 0.075f);
    const int trailWidth = gWidth / 5;
    const int trailOffset = static_cast<int>((steer + 1.0f) * (gWidth - trailWidth) / 2.0f);
    clearPanel(trailOffset, 0, trailWidth, gHeight / 2, 0.30f + throttle * 0.18f, 0.20f, 0.08f);
    clearPanel(0, 0, gWidth / 2, gHeight / 5, 0.10f, 0.10f, 0.13f);
    clearPanel(gWidth / 2, gHeight * 4 / 5, gWidth / 2, gHeight / 5, 0.15f, 0.23f, 0.12f);
    clearPanel(gWidth / 2, 0, gWidth / 2, gHeight / 5, 0.25f, 0.11f + brake * 0.16f, 0.08f);
}
