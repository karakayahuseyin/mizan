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

#endif // APPLICATION_H