#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "../gfx/shaders.hpp"

#include <vector>
#include <filesystem>
#include <string>

namespace renderer {

class Pipeline {
public:
    struct Builder {
        std::vector<std::string> shaders;

        Builder& addShader(std::filesystem::path path);
        Pipeline build();
    };

    // Pipeline();
    // ~Pipeline();

private:

};

} // namespace renderer

#endif