#include "Window.h"
#include <iostream>

Window::Window(int width, int height, const std::string& title)
    : m_window(nullptr), m_width(width), m_height(height), m_title(title), m_firstMouse(true), m_scrollDelta(0.0) {
    // Initialize mouse button states
    for (int i = 0; i < 3; ++i) {
        m_mouseButtonPressed[i] = false;
    }
}

Window::~Window() {
    cleanup();
}

bool Window::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return false;
    }

    glViewport(0, 0, m_width, m_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    if (!initImGui()) {
        std::cerr << "Failed to initialize ImGui\n";
        return false;
    }
    
    // Set up mouse callbacks
    setMouseCallbacks();

    return true;
}

void Window::cleanup() {
    cleanupImGui();
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

bool Window::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Initialize ImGui GLFW implementation with install_callbacks=false
    // We'll handle callbacks ourselves to properly integrate with our input system
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, false)) {
        return false;
    }
    
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        return false;
    }

    return true;
}

void Window::cleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Window::getMousePosition(double& x, double& y) const {
    glfwGetCursorPos(m_window, &x, &y);
}

void Window::getMouseDelta(double& deltaX, double& deltaY) {
    double currentX, currentY;
    glfwGetCursorPos(m_window, &currentX, &currentY);
    
    if (m_firstMouse) {
        m_lastMouseX = currentX;
        m_lastMouseY = currentY;
        const_cast<Window*>(this)->m_firstMouse = false;
    }
    
    deltaX = currentX - m_lastMouseX;
    deltaY = m_lastMouseY - currentY; // Reversed since y-coordinates go from bottom to top
    
    const_cast<Window*>(this)->m_lastMouseX = currentX;
    const_cast<Window*>(this)->m_lastMouseY = currentY;
}

bool Window::isMouseButtonPressed(int button) const {
    if (button >= 0 && button < 3) {
        return m_mouseButtonPressed[button];
    }
    return false;
}

void Window::setMouseCallbacks() {
    glfwSetWindowUserPointer(m_window, this);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetCharCallback(m_window, charCallback);
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Always let ImGui handle the event first
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    
    // Check if ImGui wants to capture this input
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return; // ImGui is handling this event, don't pass to our system
    }
    
    // Now handle our custom input
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && button >= 0 && button < 3) {
        win->m_mouseButtonPressed[button] = (action == GLFW_PRESS);
    }
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Let ImGui handle cursor position
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Always let ImGui handle the event first
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    
    // Check if ImGui wants to capture this input
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return; // ImGui is handling this event
    }
    
    // Handle scroll for camera zoom
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        win->handleScroll(xoffset, yoffset);
    }
}

// Add new callback functions for keyboard input
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    
    // Check if ImGui wants to capture keyboard input
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        return; // ImGui is handling this event
    }
    
    // Handle custom keyboard shortcuts
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && action == GLFW_PRESS) {
        // Add custom key handling here if needed
        // For now, we'll let the application handle keyboard shortcuts
    }
}

void Window::charCallback(GLFWwindow* window, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(window, c);
}

void Window::handleScroll(double xoffset, double yoffset) {
    m_scrollDelta = yoffset;
}

double Window::getScrollDelta() {
    double delta = m_scrollDelta;
    m_scrollDelta = 0.0; // Reset after reading
    return delta;
}