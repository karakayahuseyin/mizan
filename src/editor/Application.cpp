#include "Application.h"

#include <iostream>

Application::Application()
    : m_window(nullptr), m_workbench(nullptr), m_running(false) {}

Application::~Application() {
    shutdown();
}

bool Application::init() {
    m_window = std::make_unique<Window>(1280, 720, "Mizan Editor");
    if (!m_window->init()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }

    m_workbench = std::make_unique<Workbench>(m_window.get());
    if (!m_workbench->init()) {
        return false;
    }

    return true;
}

void Application::run() {
    m_running = true;
    while (m_running && !m_window->shouldClose()) {
        m_window->pollEvents();
        m_workbench->update();
        m_window->swapBuffers();
    }

    shutdown();
}

void Application::shutdown() {
    if (m_workbench) {
        m_workbench->cleanup();
        m_workbench.reset();
    }

    if (m_window) {
        m_window->cleanup();
        m_window.reset();
    }

    m_running = false;
}