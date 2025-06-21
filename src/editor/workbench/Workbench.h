#ifndef WORKBENCH_H
#define WORKBENCH_H

#include "Toolkit.h"
#include "Viewport.h"
#include "window/Window.h"

#include <memory>

class Workbench {
public:
    Workbench(Window* window, Scene* scene = nullptr);
    ~Workbench();

    bool init();
    void update();
    void cleanup();

private:
    Window* m_window = nullptr; // Pointer to the main application window
    Scene* m_scene = nullptr; // Pointer to the scene containing all objects
    std::unique_ptr<Toolkit> m_toolkit;
    std::unique_ptr<Viewport> m_viewport;
};

#endif // WORKBENCH_H