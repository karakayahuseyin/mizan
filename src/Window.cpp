#include "Window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Input callback storage
static double g_scrollDelta = 0.0;

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    g_scrollDelta += yoffset;
    
    // Forward to ImGui
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Forward to ImGui
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Forward to ImGui
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

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
    
    // Set OpenGL context hints before window creation
    glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create windowed mode window and its OpenGL context
    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(m_window);
    
    // Setup input callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    
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
    
    // Configure OpenGL for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    
    return true;
}

void Window::pollEvents() {
    // Update mouse delta
    double currentMouseX, currentMouseY;
    glfwGetCursorPos(m_window, &currentMouseX, &currentMouseY);
    
    if (m_firstMouse) {
        m_lastMouseX = currentMouseX;
        m_lastMouseY = currentMouseY;
        m_firstMouse = false;
    }
    
    m_mouseDeltaX = currentMouseX - m_lastMouseX;
    m_mouseDeltaY = currentMouseY - m_lastMouseY;
    
    m_lastMouseX = currentMouseX;
    m_lastMouseY = currentMouseY;
    
    // Poll for window events
    glfwPollEvents();
}

void Window::getMousePosition(double& x, double& y) {
    glfwGetCursorPos(m_window, &x, &y);
}

void Window::setMousePosition(double x, double y) {
    glfwSetCursorPos(m_window, x, y);
}

void Window::getMouseDelta(double& dx, double& dy) {
    dx = m_mouseDeltaX;
    dy = m_mouseDeltaY;
    // Reset delta after it's been read
    m_mouseDeltaX = 0;
    m_mouseDeltaY = 0;
}

bool Window::isMouseButtonPressed(int button) {
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

double Window::getScrollDelta() {
    double delta = g_scrollDelta;
    g_scrollDelta = 0; // Reset after reading
    return delta;
}

void Window::resetScrollDelta() {
    g_scrollDelta = 0;
}

bool Window::isKeyPressed(int key) {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void Window::beginImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::endImGuiFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::clear() {
    // Clear both color and depth buffer with proper values
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClearDepth(1.0f);
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

void Window::cleanup() {
    if (m_window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}