#include "Toolkit.h"
#include "brep/Builder.h"
#include "brep/Solid.h"
#include "renderer/Tessellator.h"
#include "logger/Logger.h"
#include "ui/Icons.h"
#include "ui/IconUtils.h"

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

    renderMenuBar();
    renderToolPanel();
    
    // Render settings window if open
    Settings::getInstance().renderSettingsWindow();
    
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

void Toolkit::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (IconUtils::IconMenuItem(Icons::FILE, "New", "Ctrl+N")) {
                // Handle new file
            }
            if (IconUtils::IconMenuItem(Icons::FOLDER, "Open", "Ctrl+O")) {
                // Handle open file
            }
            if (IconUtils::IconMenuItem(Icons::SAVE, "Save", "Ctrl+S")) {
                // Handle save file
            }
            ImGui::Separator();
            if (IconUtils::IconMenuItem(Icons::CROSS, "Exit", "Alt+F4")) {
                // Handle exit
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            Settings& settings = Settings::getInstance();
            if (IconUtils::IconMenuItem(Icons::SETTINGS, "Settings")) {
                settings.setSettingsWindowOpen(true);
            }
            ImGui::Separator();
            if (IconUtils::IconMenuItem(Icons::HOME, "Reset Camera")) {
                // Could add camera reset functionality here
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {
            if (IconUtils::IconMenuItem(Icons::REDO, "Reset Grid")) {
                Settings& settings = Settings::getInstance();
                settings.setGridSize(20.0f);
                settings.setGridSpacing(0.5f);
                settings.setGridColor(glm::vec3(0.5f, 0.5f, 0.5f));
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Toolkit::renderToolPanel() {
    // Create a tools panel window
    if (ImGui::Begin("Tools")) {
        // Primitive Objects section
        if (IconUtils::IconCollapsingHeader(Icons::CUBE, "Primitive Objects", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Spacing();
            
            // Use IconButton for better visual appeal
            if (IconUtils::IconButton(Icons::CUBE, "Add Cube")) {
                addSolid("Cube", BREP::Solid::PrimitiveType::Cube);
            }
            
            if (IconUtils::IconButton(Icons::SPHERE, "Add Sphere")) {
                addSolid("Sphere", BREP::Solid::PrimitiveType::Sphere);
            }
            
            if (IconUtils::IconButton(Icons::CYLINDER, "Add Cylinder")) {
                addSolid("Cylinder", BREP::Solid::PrimitiveType::Cylinder);
            }
            
            if (IconUtils::IconButton(Icons::CONE, "Add Pyramid")) {
                addSolid("Pyramid", BREP::Solid::PrimitiveType::Pyramid);
            }
        }
        
        ImGui::Separator();
        
        // Grid Settings section
        if (IconUtils::IconCollapsingHeader(Icons::VIEW, "Grid Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Spacing();
            
            Settings& settings = Settings::getInstance();
            bool gridEnabled = settings.isGridEnabled();
            if (ImGui::Checkbox("Show Grid", &gridEnabled)) {
                settings.setGridEnabled(gridEnabled);
            }
        }
    }
    ImGui::End();
}