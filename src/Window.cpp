#include "Window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(int width, int height, const char* title) 
    : m_width(width), m_height(height), m_title(title), m_window(nullptr) {
}

Window::~Window() {
    cleanup();
}

bool Window::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Create windowed mode window and its OpenGL context
    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(m_window);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    
    // Set viewport
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    return true;
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::setTitle(const char* title) {
    glfwSetWindowTitle(m_window, title);
}

void Window::getSize(int& width, int& height) const {
    glfwGetWindowSize(m_window, &width, &height);
}

void Window::setMousePosition(double x, double y) {
    glfwSetCursorPos(m_window, x, y);
}

void Window::getMousePosition(double& x, double& y) {
    glfwGetCursorPos(m_window, &x, &y);
}

void Window::cleanup() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}