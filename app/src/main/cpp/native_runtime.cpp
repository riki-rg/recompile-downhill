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
constexpr std::size_t kMaxEntries = 32;

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

    std::vector<std::uint8_t> directory(rootSize);
    if (!readFully(fd, static_cast<off_t>(rootSector) * kSectorSize, directory.data(), directory.size())) return "Could not read the ISO-9660 root directory.";

    std::string report = "ISO-9660 volume: " + volume + "\n\nRoot entries:";
    std::size_t cursor = 0, entries = 0;
    bool truncated = false;
    while (cursor < directory.size()) {
        const std::uint8_t length = directory[cursor];
        if (length == 0) { cursor = ((cursor / kSectorSize) + 1) * kSectorSize; continue; }
        if (length < 34 || cursor + length > directory.size()) return "ISO-9660 directory contains a malformed record.";
        const auto* record = directory.data() + cursor;
        const std::uint8_t nameLength = record[32];
        if (33 + nameLength > length) return "ISO-9660 directory contains an invalid filename.";
        if (nameLength != 1 || (record[33] != 0 && record[33] != 1)) {
            if (entries == kMaxEntries) { truncated = true; break; }
            report += "\n" + cleanText(record + 33, nameLength) + "  |  " + std::to_string(readLittleEndian32(record + 10)) + " B";
            ++entries;
        }
        cursor += length;
    }
    if (entries == 0) report += "\n(no visible root entries)";
    if (truncated) report += "\n\nDirectory is large; showing the first 32 entries only.";
    return report + "\n\nDownhill Domination USA compatibility has not yet been verified.";
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
