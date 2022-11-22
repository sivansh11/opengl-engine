#include "window.hpp"

#include <glad/glad.h>

#include <iostream>

namespace core {

void debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar *message, const void *userParam)
{
    auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
        throw std::runtime_error("Unknown Source!");
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
        throw std::runtime_error("Unknown Type!");
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
        throw std::runtime_error("Unknown Severity!");
	}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        // throw std::runtime_error("");
    } 
}    

Window::Window(const char *name, uint32_t width, uint32_t height)
    : m_name(name),
      m_width(width),
      m_height(height) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw!");
    }
    #ifndef NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    #else 
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
    #endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(static_cast<int>(m_width),
                                static_cast<int>(m_height), 
                                m_name.c_str(),
                                NULL,
                                NULL);
    if (!m_window) {
        throw std::runtime_error("Failed to create window!");
    }    

    glfwMakeContextCurrent(m_window);                     
    glfwSwapInterval(1);

    if (!gladLoadGL()) {
        throw std::runtime_error("Failed to load opengl!");
    }

    #ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugMessage, NULL);

        // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    #else

    #endif
}

Window::~Window() {

    glfwTerminate();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::updateScreen() {
    glfwSwapBuffers(m_window);
}

} // namespace core
