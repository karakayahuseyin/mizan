#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <memory>

class Window {
private:
    int m_width, m_height;
    const char* m_title;
    GLFWwindow* m_window;
    
public:
    Window(int width, int height, const char* title);
    ~Window();
    
    bool init();
    void cleanup();
    
    void pollEvents();
    void swapBuffers();
    
    bool shouldClose() const;
    
    // Mouse input
    bool isMouseButtonPressed(int button) const;
    void getMousePosition(double& x, double& y);
    void getMouseDelta(double& x, double& y);
    
    // Keyboard input
    bool isKeyPressed(int key) const;
    
    // Getters
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    GLFWwindow* getGLFWwindow() const { return m_window; }
};

#endif // WINDOW_H