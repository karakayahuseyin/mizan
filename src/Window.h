#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <memory>

class Window {
private:
    int m_width, m_height;
    const char* m_title;
    GLFWwindow* m_window;
    
    // Mouse delta tracking
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    double m_mouseDeltaX = 0.0;
    double m_mouseDeltaY = 0.0;
    bool m_firstMouse = true;
    
    // Static callback functions
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    
public:
    Window(int width, int height, const char* title);
    ~Window();
    
    bool init();
    void cleanup();
    
    void pollEvents();
    void swapBuffers();
    void clear();
    
    bool shouldClose() const;
    
    // Mouse input
    bool isMouseButtonPressed(int button) const;
    void getMousePosition(double& x, double& y);
    void setMousePosition(double x, double y);
    void getMouseDelta(double& x, double& y);
    double getScrollDelta();
    void resetScrollDelta();
    
    // Keyboard input
    bool isKeyPressed(int key) const;
    
    // ImGui frame management
    void beginImGuiFrame();
    void endImGuiFrame();
    
    // Window management
    void setTitle(const char* title);
    void getSize(int& width, int& height) const;
    
    // Getters
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    GLFWwindow* getGLFWwindow() const { return m_window; }
};

#endif // WINDOW_H