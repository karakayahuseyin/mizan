#include "Workbench.h"

Workbench::Workbench(Window* window, Scene* scene) 
    : m_window(window), m_scene(scene), m_toolkit(nullptr), m_viewport(nullptr) {}

Workbench::~Workbench() {
    cleanup();
}

bool Workbench::init() {

    m_scene = new Scene();
    m_toolkit = std::make_unique<Toolkit>(m_window, m_scene);
    m_viewport = std::make_unique<Viewport>(m_window, m_scene);
    
    // Initialize toolkit and viewport
    m_toolkit->initialize();
    m_viewport->initialize();
    
    // Setup any additional resources or state

    return true;
}

void Workbench::update() {
    if (m_viewport) {
        m_viewport->render();
    }
    
    if (m_toolkit) {
        m_toolkit->update();
        m_toolkit->render();
    }
}

void Workbench::cleanup() {
    // Cleanup viewport and toolkit
    if (m_viewport) {
        m_viewport->cleanup();
    }
    
    if (m_toolkit) {
        m_toolkit->cleanup();
    }

    delete m_window;
    
    // m_viewport.reset();
    // m_toolkit.reset();
}