#include "Toolkit.h"

Toolkit::Toolkit(Window* window, Scene* scene)
    : m_window(window), m_scene(scene), m_modeller(nullptr) {
    // Constructor implementation
}

Toolkit::~Toolkit() {
    // Destructor implementation
    if (m_modeller) {
        delete m_modeller; // Clean up the Modeller instance if it was created
    }
}

void Toolkit::initialize() {
    // Initialization code for the toolkit
}

void Toolkit::update() {
    // Update logic for the toolkit
}

void Toolkit::render() {
    m_window->beginImGuiFrame();

    ImGui::Begin("Toolkit");
    ImGui::Text("Welcome to the Mizan Editor Toolkit!");
    ImGui::Text("This toolkit provides various tools for modelling and scene management.");
    ImGui::End();

    m_window->endImGuiFrame();
}

void Toolkit::cleanup() {
    // Cleanup code for the toolkit
}

