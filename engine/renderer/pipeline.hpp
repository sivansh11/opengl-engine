#ifndef GFX_PIPELINE_HPP
#define GFX_PIPELINE_HPP

#include <entt/entt.hpp>

namespace gfx {

class Pipeline {
public:
    Pipeline();
    ~Pipeline();

    void bind(entt::registry& registry);

private:

};

} // namespace gfx

#endif