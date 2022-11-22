#include "app.hpp"

#include "shaders.hpp"
#include "buffer.hpp"

#include <glm/glm.hpp>

std::ostream& operator<<(std::ostream& out, const glm::vec3& vec) {
    out << vec.x << ' ' << vec.y << ' ' << vec.z;
    return out;
}


int main() {

    app::App *app = new app::App();

    app->run();

    delete app;

    return 0;
}