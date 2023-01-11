#include "utils.hpp"

#include <fstream>

namespace core {

std::string readFile(const std::string& filePath, FileType type) {
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to read file!");
    }
    return std::string((std::istreambuf_iterator<char>(ifs)),
                  (std::istreambuf_iterator<char>()));
} 

std::ostream& operator<<(std::ostream& out, const glm::vec3& vec) {
    out << vec.x << ' ' << vec.y << ' ' << vec.z;
    return out;
}

} // namespace core
