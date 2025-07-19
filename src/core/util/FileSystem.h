#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace core::util {

/* Minimal static helpers – grow as needed */
class FileSystem {
  public:
    /* Entire file → std::vector<char>.  Throws on failure. */
    static std::vector<char> readBinary(const std::filesystem::path& p) {
        std::ifstream file(p, std::ios::binary | std::ios::ate);
        if (!file)
            throw std::runtime_error("Failed to open file: " + p.string());

        const std::size_t size = static_cast<std::size_t>(file.tellg());
        std::vector<char> data(size);
        file.seekg(0);
        file.read(data.data(), size);
        return data;
    }

    static bool exists(const std::filesystem::path& p) noexcept {
        return std::filesystem::exists(p);
    }

    static std::filesystem::path exeDirectory() {
        return std::filesystem::current_path(); // fine for now
    }
};

} // namespace core::util
