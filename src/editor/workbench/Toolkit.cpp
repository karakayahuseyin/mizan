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

    ImGui::Begin("Toolkit");
    ImGui::Text("Welcome to the Mizan Editor Toolkit!");
    ImGui::Text("This toolkit provides various tools for modelling and scene management.");
    ImGui::End();

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
        // Scene is null, cannot add objects
        return;
    }

    if (!m_modeller) {
        m_modeller = new mizan::Modeller();
    }

    try {
        SceneObject newObject;
        newObject.name = name;

        // Create solid with error checking
        newObject.solid = BREP::Builder::createSolid(type);

        // Tessellate with error checking
        newObject.mesh = Tessellator::tessellate(newObject.solid);

        // Validate mesh before proceeding
        if (newObject.mesh.getVertexCount() == 0) {
            // Empty mesh, skip adding
            return;
        }

        newObject.mesh.setColor(0.75f, 0.75f, 0.75f);
        newObject.mesh.m_showWireframe = true;
        newObject.mesh.m_showSolid = true;

        m_scene->addObject(newObject);
    } catch (const std::exception& e) {
        Logger::log(e.what(), Logger::LogLevel::Error);
    }
}