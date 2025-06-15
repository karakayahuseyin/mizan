#include "UIManager.h"
#include "Window.h"
#include "../ui/Viewport.h"
#include "../Application.h"
#include <cstring>

UIManager::UIManager(Window* window, Viewport* viewport)
    : m_window(window), m_viewport(viewport) {
}

UIManager::~UIManager() {
}

void UIManager::render() {
    // Handle keyboard shortcuts first
    handleKeyboardShortcuts();
    
    // Begin ImGui frame for UI
    m_window->beginImGuiFrame();
    
    // Render UI components
    renderMainMenuBar();
    renderObjectList();
    renderObjectProperties();
    renderViewportControls();
    renderKeyboardShortcuts();
    
    // End ImGui frame
    m_window->endImGuiFrame();
}

void UIManager::handleKeyboardShortcuts() {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard) {
        if (ImGui::IsKeyPressed(ImGuiKey_W)) {
            m_globalWireframeMode = !m_globalWireframeMode;
            if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(m_globalWireframeMode);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S)) {
            m_globalSolidMode = !m_globalSolidMode;
            if (onSetGlobalSolidMode) onSetGlobalSolidMode(m_globalSolidMode);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_1)) {
            // Wireframe only
            m_globalWireframeMode = true;
            m_globalSolidMode = false;
            if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(true);
            if (onSetGlobalSolidMode) onSetGlobalSolidMode(false);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_2)) {
            // Solid only
            m_globalWireframeMode = false;
            m_globalSolidMode = true;
            if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(false);
            if (onSetGlobalSolidMode) onSetGlobalSolidMode(true);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_3)) {
            // Both wireframe and solid
            m_globalWireframeMode = true;
            m_globalSolidMode = true;
            if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(true);
            if (onSetGlobalSolidMode) onSetGlobalSolidMode(true);
        }
    }
}

void UIManager::renderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {
                if (onNewScene) onNewScene();
            }
            if (ImGui::MenuItem("Exit")) {
                if (onExit) onExit();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem("Cube")) {
                if (onAddObject) onAddObject("Cube");
            }
            if (ImGui::MenuItem("Pyramid")) {
                if (onAddObject) onAddObject("Pyramid");
            }
            if (ImGui::MenuItem("Sphere")) {
                if (onAddObject) onAddObject("Sphere");
            }
            if (ImGui::MenuItem("Cylinder")) {
                if (onAddObject) onAddObject("Cylinder");
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Object List", nullptr, &m_showObjectList);
            ImGui::MenuItem("Properties", nullptr, &m_showObjectProperties);
            ImGui::MenuItem("Grid", nullptr, &m_showGrid);
            
            ImGui::Separator();
            
            // Global rendering modes
            if (ImGui::MenuItem("Wireframe Mode", "W", m_globalWireframeMode)) {
                m_globalWireframeMode = !m_globalWireframeMode;
                if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(m_globalWireframeMode);
            }
            if (ImGui::MenuItem("Solid Mode", "S", m_globalSolidMode)) {
                m_globalSolidMode = !m_globalSolidMode;
                if (onSetGlobalSolidMode) onSetGlobalSolidMode(m_globalSolidMode);
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Wireframe Only", nullptr, m_globalWireframeMode && !m_globalSolidMode)) {
                m_globalWireframeMode = true;
                m_globalSolidMode = false;
                if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(true);
                if (onSetGlobalSolidMode) onSetGlobalSolidMode(false);
            }
            if (ImGui::MenuItem("Solid Only", nullptr, !m_globalWireframeMode && m_globalSolidMode)) {
                m_globalWireframeMode = false;
                m_globalSolidMode = true;
                if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(false);
                if (onSetGlobalSolidMode) onSetGlobalSolidMode(true);
            }
            if (ImGui::MenuItem("Wireframe + Solid", nullptr, m_globalWireframeMode && m_globalSolidMode)) {
                m_globalWireframeMode = true;
                m_globalSolidMode = true;
                if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(true);
                if (onSetGlobalSolidMode) onSetGlobalSolidMode(true);
            }
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void UIManager::renderObjectList() {
    if (!m_showObjectList) return;
    
    ImGui::Begin("Scene Objects", &m_showObjectList);
    
    if (onGetSceneObjects) {
        const auto& sceneObjects = onGetSceneObjects();
        SceneObject* selectedObj = onGetSelectedObject ? onGetSelectedObject() : nullptr;
        int selectedId = selectedObj ? selectedObj->id : -1;
        
        for (const auto& obj : sceneObjects) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
            if (obj.id == selectedId) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            
            bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)obj.id, flags, "%s", obj.name.c_str());
            
            if (ImGui::IsItemClicked()) {
                if (onSelectObject) onSelectObject(obj.id);
            }
            
            // Context menu
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete")) {
                    if (onRemoveObject) onRemoveObject(obj.id);
                }
                ImGui::EndPopup();
            }
            
            if (nodeOpen) {
                ImGui::TreePop();
            }
        }
    }
    
    ImGui::End();
}

void UIManager::renderObjectProperties() {
    if (!m_showObjectProperties) return;
    
    ImGui::Begin("Properties", &m_showObjectProperties);
    
    SceneObject* selectedObj = onGetSelectedObject ? onGetSelectedObject() : nullptr;
    if (selectedObj) {
        bool meshChanged = false;
        
        // Object name
        char nameBuffer[256];
        strncpy(nameBuffer, selectedObj->name.c_str(), sizeof(nameBuffer));
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            selectedObj->name = nameBuffer;
        }
        
        // Visibility
        ImGui::Checkbox("Visible", &selectedObj->visible);
        
        // Transform
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            float pos[3] = {selectedObj->mesh.m_position[0], selectedObj->mesh.m_position[1], selectedObj->mesh.m_position[2]};
            if (ImGui::DragFloat3("Position", pos, 0.1f)) {
                selectedObj->mesh.setPosition(pos[0], pos[1], pos[2]);
                meshChanged = true;
            }
            
            float rot[3] = {selectedObj->mesh.m_rotation[0], selectedObj->mesh.m_rotation[1], selectedObj->mesh.m_rotation[2]};
            if (ImGui::DragFloat3("Rotation", rot, 1.0f)) {
                selectedObj->mesh.setRotation(rot[0], rot[1], rot[2]);
                meshChanged = true;
            }
            
            float scale[3] = {selectedObj->mesh.m_scale[0], selectedObj->mesh.m_scale[1], selectedObj->mesh.m_scale[2]};
            if (ImGui::DragFloat3("Scale", scale, 0.1f, 0.1f, 10.0f)) {
                selectedObj->mesh.setScale(scale[0], scale[1], scale[2]);
                meshChanged = true;
            }
        }
        
        // Material
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
            float color[3] = {selectedObj->mesh.m_color[0], selectedObj->mesh.m_color[1], selectedObj->mesh.m_color[2]};
            if (ImGui::ColorEdit3("Color", color)) {
                selectedObj->mesh.setColor(color[0], color[1], color[2]);
                meshChanged = true;
            }
        }
        
        // Rendering
        if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Show Solid", &selectedObj->mesh.m_showSolid)) {
                meshChanged = true;
            }
            
            if (ImGui::Checkbox("Show Wireframe", &selectedObj->mesh.m_showWireframe)) {
                meshChanged = true;
            }
            
            float wireframeColor[3] = {selectedObj->mesh.m_wireframeColor[0], selectedObj->mesh.m_wireframeColor[1], selectedObj->mesh.m_wireframeColor[2]};
            if (ImGui::ColorEdit3("Wireframe Color", wireframeColor)) {
                selectedObj->mesh.setWireframeColor(wireframeColor[0], wireframeColor[1], wireframeColor[2]);
                meshChanged = true;
            }
        }
        
        // Sync changes to viewport immediately
        if (meshChanged && onSyncSelectedObjectMesh) {
            onSyncSelectedObjectMesh();
        }
    } else {
        ImGui::Text("No object selected");
    }
    
    ImGui::End();
}

void UIManager::renderViewportControls() {
    ImGui::Begin("Viewport Controls");
    
    ImGui::Text("Camera Controls:");
    ImGui::Text("Left Mouse: Rotate");
    ImGui::Text("Right Mouse: Pan");
    ImGui::Text("Scroll: Zoom");
    
    ImGui::Separator();
    
    // Rendering mode controls
    ImGui::Text("Rendering Mode:");
    
    if (ImGui::Button("Wireframe Only")) {
        m_globalWireframeMode = true;
        m_globalSolidMode = false;
        if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(true);
        if (onSetGlobalSolidMode) onSetGlobalSolidMode(false);
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Solid Only")) {
        m_globalWireframeMode = false;
        m_globalSolidMode = true;
        if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(false);
        if (onSetGlobalSolidMode) onSetGlobalSolidMode(true);
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Both")) {
        m_globalWireframeMode = true;
        m_globalSolidMode = true;
        if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(true);
        if (onSetGlobalSolidMode) onSetGlobalSolidMode(true);
    }
    
    ImGui::Separator();
    
    // Individual toggles
    if (ImGui::Checkbox("Show Wireframe", &m_globalWireframeMode)) {
        if (onSetGlobalWireframeMode) onSetGlobalWireframeMode(m_globalWireframeMode);
    }
    
    if (ImGui::Checkbox("Show Solid", &m_globalSolidMode)) {
        if (onSetGlobalSolidMode) onSetGlobalSolidMode(m_globalSolidMode);
    }
    
    // Grid control
    if (ImGui::Checkbox("Show Grid", &m_showGrid)) {
        if (m_viewport) m_viewport->enableGrid(m_showGrid);
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Reset Camera")) {
        if (m_viewport) m_viewport->resetCamera();
    }
    
    ImGui::End();
}

void UIManager::renderKeyboardShortcuts() {
    ImGui::Begin("Keyboard Shortcuts");
    ImGui::Text("W - Toggle Wireframe");
    ImGui::Text("S - Toggle Solid");
    ImGui::Text("1 - Wireframe Only");
    ImGui::Text("2 - Solid Only");
    ImGui::Text("3 - Wireframe + Solid");
    ImGui::End();
}
