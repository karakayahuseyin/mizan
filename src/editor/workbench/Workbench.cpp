#include "Workbench.h"

Workbench::Workbench() {
    initialize();
}

Workbench::~Workbench() {
    cleanup();
}

void Workbench::initialize() {
    m_toolkit = std::make_unique<Toolkit>();
    m_viewport = std::make_unique<Viewport>();
    
    // Initialize toolkit and viewport
    m_toolkit->initialize();
    m_viewport->initialize();
    
    // Setup any additional resources or state
}

void Workbench::cleanup() {
    // Cleanup viewport and toolkit
    if (m_viewport) {
        m_viewport->cleanup();
    }
    
    if (m_toolkit) {
        m_toolkit->cleanup();
    }
    
    // m_viewport.reset();
    // m_toolkit.reset();
}