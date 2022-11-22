#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "imgui.h"
#include "event_types.hpp"
#include "ecs.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/constants.hpp>

namespace core {

class Controller
{
public:
    struct KeyMappings 
    {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_SPACE;
        int moveDown = GLFW_KEY_LEFT_SHIFT;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
    };

    void move(GLFWwindow* window, float dt, glm::vec3 &translation, glm::vec3 &rotation, glm::vec2 size, glm::vec2 offset);

    glm::vec3 up{0, 1, 0};
    float moveSpeed{4.f};
    float lookSpeed{50.f};
    KeyMappings keys;
    glm::vec2 size;
    glm::vec2 offset;

    static void componentPanel(Controller& controller);

private:
    bool firstClick = true;
};

} // namespace core

#endif