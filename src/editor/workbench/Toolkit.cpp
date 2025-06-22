#include "Toolkit.h"
#include "brep/Builder.h"
#include "brep/Solid.h"
#include "renderer/Tessellator.h"
#include "logger/Logger.h"

#include <string>

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

    // Add a button to create a new solid
    if (ImGui::Button("Add Cube")) {
        addSolid("Cube", BREP::Solid::PrimitiveType::Cube);
    }
    if (ImGui::Button("Add Sphere")) {
        addSolid("Sphere", BREP::Solid::PrimitiveType::Sphere);
    }
    if (ImGui::Button("Add Cylinder")) {
        addSolid("Cylinder", BREP::Solid::PrimitiveType::Cylinder);
    }
    if (ImGui::Button("Add Pyramid")) {
        addSolid("Pyramid", BREP::Solid::PrimitiveType::Pyramid);
    }
    
    m_window->endImGuiFrame();
}

void Toolkit::cleanup() {
    // Cleanup code for the toolkit
}

void Toolkit::addSolid(std::string name, BREP::Solid::PrimitiveType type) {
    if (!m_scene) {
        Logger::error("Scene is not initialized. Cannot add solid.");
        return;
    }

    SceneObject newObject;
    newObject.name = name;

    newObject.solid = BREP::Builder::createSolid(type);
    newObject.mesh = Tessellator::tessellate(newObject.solid);

    if (newObject.mesh.getVertexCount() == 0) {
        Logger::error("Tessellation failed. Mesh is empty. Cannot add solid.");
        return;
    }

    newObject.mesh.setColor(0.75f, 0.75f, 0.75f);
    newObject.mesh.m_showWireframe = true;
    newObject.mesh.m_showSolid = true;

    if (m_scene->addObject(newObject)) {
        Logger::info("Solid added successfully.");
    } else {
        Logger::error("Failed to add solid.");
    }
}