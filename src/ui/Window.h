#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>

class Window {
private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
    
    // Mouse input state
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_firstMouse = true;
    bool m_mouseButtonPressed[3] = {false, false, false}; // Left, Right, Middle
    
    // Scroll state
    double m_scrollDelta = 0.0;

public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool init();
    void cleanup();
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void clear();
    
    void beginImGuiFrame();
    void endImGuiFrame();
    
    GLFWwindow* getGLFWWindow() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    // Mouse input methods
    void getMousePosition(double& x, double& y) const;
    void getMouseDelta(double& deltaX, double& deltaY);
    bool isMouseButtonPressed(int button) const;
    void setMouseCallbacks();
    
    // Scroll handling
    void handleScroll(double xoffset, double yoffset);
    double getScrollDelta();

private:
    bool initImGui();
    void cleanupImGui();
    
    // Static callbacks for GLFW
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void charCallback(GLFWwindow* window, unsigned int c);
};

#endif // UI_WINDOW_H