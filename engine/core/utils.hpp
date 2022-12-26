#ifndef UTILS_HPP
#define UTILS_HPP

#include <glm/glm.hpp>

#include <iostream>
#include <string>

namespace core {

enum FileType {
    TEXT,
    BINARY
};

std::string readFile(const char *filePath, FileType type);

template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
}

std::ostream& operator<<(std::ostream& out, const glm::vec3& vec);

} // namespace core

#endif