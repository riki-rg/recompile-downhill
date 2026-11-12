#include <jni.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdio>
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
        if (count <= 0) {
            return false;
        }
        total += static_cast<std::size_t>(count);
    }
    return true;
}

std::uint32_t readLittleEndian32(const std::uint8_t* value) {
    return static_cast<std::uint32_t>(value[0])
           | (static_cast<std::uint32_t>(value[1]) << 8)
           | (static_cast<std::uint32_t>(value[2]) << 16)
           | (static_cast<std::uint32_t>(value[3]) << 24);
}

std::string cleanText(const std::uint8_t* bytes, std::size_t length) {
    std::string value;
    for (std::size_t index = 0; index < length; ++index) {
        const unsigned char character = bytes[index];
        if (character == ' ') {
            continue;
        }
        value.push_back(std::isprint(character) ? static_cast<char>(character) : '?');
    }
    return value.empty() ? "UNTITLED" : value;
}

std::string formatSize(std::uint32_t bytes) {
    return std::to_string(bytes) + " B";
}

std::string inspectIso(int fd) {
    std::array<std::uint8_t, kSectorSize> pvd{};
    if (!readFully(fd, kPvdOffset, pvd.data(), pvd.size())) {
        return "Could not read the ISO-9660 primary volume descriptor.";
    }
    if (pvd[0] != 1 || std::string(reinterpret_cast<const char*>(pvd.data() + 1), 5) != "CD001" || pvd[6] != 1) {
        return "This file is not a readable ISO-9660 image.";
    }

    const std::string volume = cleanText(pvd.data() + 40, 32);
    const std::uint8_t recordLength = pvd[156];
    if (recordLength < 34 || 156 + recordLength > pvd.size()) {
        return "ISO-9660 root directory metadata is malformed.";
    }
    const auto* root = pvd.data() + 156;
    const std::uint32_t rootSector = readLittleEndian32(root + 2);
    const std::uint32_t rootSize = readLittleEndian32(root + 10);
    if (rootSize == 0 || rootSize > 8 * 1024 * 1024) {
        return "ISO-9660 root directory size is unsupported.";
    }

    std::vector<std::uint8_t> directory(rootSize);
    if (!readFully(fd, static_cast<off_t>(rootSector) * kSectorSize, directory.data(), directory.size())) {
        return "Could not read the ISO-9660 root directory.";
    }

    std::string report = "ISO-9660 volume: " + volume + "\n\nRoot entries:";
    std::size_t cursor = 0;
    std::size_t entries = 0;
    bool truncated = false;
    while (cursor < directory.size()) {
        const std::uint8_t length = directory[cursor];
        if (length == 0) {
            cursor = ((cursor / kSectorSize) + 1) * kSectorSize;
            continue;
        }
        if (length < 34 || cursor + length > directory.size()) {
            return "ISO-9660 directory contains a malformed record.";
        }
        const auto* record = directory.data() + cursor;
        const std::uint8_t nameLength = record[32];
        if (33 + nameLength > length) {
            return "ISO-9660 directory contains an invalid filename.";
        }
        if (nameLength != 1 || (record[33] != 0 && record[33] != 1)) {
            if (entries == kMaxEntries) {
                truncated = true;
                break;
            }
            const std::string name = cleanText(record + 33, nameLength);
            const std::uint32_t size = readLittleEndian32(record + 10);
            report += "\n" + name + "  |  " + formatSize(size);
            ++entries;
        }
        cursor += length;
    }
    if (entries == 0) {
        report += "\n(no visible root entries)";
    }
    if (truncated) {
        report += "\n\nDirectory is large; showing the first 32 entries only.";
    }
    report += "\n\nDownhill Domination USA compatibility has not yet been verified.";
    return report;
}
}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_id_dev_rikirg_downhillnative_MainActivity_nativeRuntimeName(JNIEnv* environment, jclass) {
    return environment->NewStringUTF("C++20 / Android NDK");
}

extern "C" JNIEXPORT jstring JNICALL
Java_id_dev_rikirg_downhillnative_MainActivity_nativeExploreIso(JNIEnv* environment, jclass, jint fileDescriptor) {
    const std::string result = inspectIso(fileDescriptor);
    return environment->NewStringUTF(result.c_str());
}
