/**
 * Copyright (c) 2025 Hüseyin Karakaya
 * This file is part of the Mizan project. Licensed under the MIT License (MIT).
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "window/Window.h"
#include "workbench/Workbench.h"

#include <memory>

class Application {
public:
    Application();
    ~Application();

    bool init();
    void run();
    void shutdown();

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Workbench> m_workbench;
    
    bool m_running;
};

// class Application {
// private:
//     std::unique_ptr<Window> m_window;
//     std::unique_ptr<Workbench> m_workbench;
    
//     bool m_running;
    
//     // Global rendering modes --Viewport.h--
//     bool m_globalWireframeMode = false;
//     bool m_globalSolidMode = true;

// public:
//     Application();
//     ~Application();

//     bool init();
//     void run();
//     void shutdown();
    
//     // Public interface for main to create test objects
//     void addTestObjects(); // --Scene.h--

// private:
//     void update();
//     void render();
//     void createTestMesh();
    
//     // Mesh synchronization
//     void syncMeshesToViewport();
//     void syncSelectedObjectMesh();
    
//     // Rendering mode management --Viewport.h--
//     void setGlobalWireframeMode(bool enabled);
//     void setGlobalSolidMode(bool enabled);
//     void toggleWireframeMode();
//     void toggleSolidMode();
    
//     void setupUICallbacks();
// };

#endif // APPLICATION_H