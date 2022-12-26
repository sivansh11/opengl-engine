#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "events.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

namespace core {

class Window {
public:
    Window(const char *name, uint32_t width, uint32_t height);
    ~Window();

    operator GLFWwindow *() {
        return m_window;
    }

    bool shouldClose()  const;
    void pollEvents();
    void updateScreen() const;

    float getAspect()   const { return float(m_width) / float(m_height); }

    float getWidth()    const { return float(m_width); }
    float getHeight()   const { return float(m_height); }

    GLFWwindow *getWindow() { return m_window; }

    event::Dispatcher& getDispatcher() { return m_dispatcher; }
    
private:
    event::Dispatcher m_dispatcher;
    GLFWwindow       *m_window;
    std::string       m_name;
    uint32_t          m_width, m_height;
};

} // namespace core


#endif