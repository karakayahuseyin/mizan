#include "Application.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>
#include <imgui.h>
#include <algorithm>
#include <cstring>

Application::Application() 
    : m_window(nullptr), m_renderer(nullptr), m_camera(nullptr), m_running(false), m_rotation(0.0f) {
    m_triangleColor[0] = 1.0f;
    m_triangleColor[1] = 0.5f;
    m_triangleColor[2] = 0.0f;
}

Application::~Application() {
    shutdown();
}

bool Application::init() {
    // Initialize window
    m_window = std::make_unique<Window>(1200, 800, "3D Modeller");
    if (!m_window->init()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }
    
    // Initialize camera
    m_camera = std::make_unique<Camera>(1200.0f / 800.0f);
    
    // Initialize renderer
    m_renderer = std::make_unique<Renderer>();
    m_renderer->initialize();
    
    // Create initial test objects
    createTestMesh();
    
    m_running = true;
    return true;
}

void Application::run() {
    while (m_running && !m_window->shouldClose()) {
        m_window->pollEvents();
        
        update();
        render();
        
        m_window->swapBuffers();
    }
}

void Application::update() {
    m_rotation += 0.01f;
    if (m_rotation > 6.28f) {
        m_rotation = 0.0f;
    }
}

void Application::render() {
    m_window->clear();
    
    // Handle mouse input
    handleMouseInput();
    
    // Handle keyboard shortcuts
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard) {
        if (ImGui::IsKeyPressed(ImGuiKey_W)) {
            toggleWireframeMode();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S)) {
            toggleSolidMode();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_1)) {
            // Wireframe only
            setGlobalWireframeMode(true);
            setGlobalSolidMode(false);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_2)) {
            // Solid only
            setGlobalWireframeMode(false);
            setGlobalSolidMode(true);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_3)) {
            // Both wireframe and solid
            setGlobalWireframeMode(true);
            setGlobalSolidMode(true);
        }
    }
    
    // Setup 3D view with camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    auto projMatrix = m_camera->getProjectionMatrix();
    glLoadMatrixf(&projMatrix[0][0]);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    auto viewMatrix = m_camera->getViewMatrix();
    glLoadMatrixf(&viewMatrix[0][0]);
    
    // Render grid
    m_renderer->enableGrid(m_showGrid);
    
    // Render 3D content
    m_renderer->render();
    
    // Begin ImGui frame for UI
    m_window->beginImGuiFrame();
    
    // Render UI
    renderMainMenuBar();
    renderObjectList();
    renderObjectProperties();
    renderViewportControls();
    
    // Show keyboard shortcuts help
    ImGui::Begin("Keyboard Shortcuts");
    ImGui::Text("W - Toggle Wireframe");
    ImGui::Text("S - Toggle Solid");
    ImGui::Text("1 - Wireframe Only");
    ImGui::Text("2 - Solid Only");
    ImGui::Text("3 - Wireframe + Solid");
    ImGui::End();
    
    // End ImGui frame
    m_window->endImGuiFrame();
}

void Application::shutdown() {
    if (m_renderer) {
        m_renderer->cleanup();
        m_renderer.reset();
    }
    
    if (m_window) {
        m_window->cleanup();
        m_window.reset();
    }
    
    m_running = false;
}

void Application::createTestMesh() {
    // Create cube
    SceneObject cubeObj;
    cubeObj.mesh = MeshGenerator::createCube(1.0f);
    cubeObj.mesh.setColor(1.0f, 0.0f, 0.0f);
    cubeObj.mesh.setPosition(-2.0f, 1.0f, 0.0f);
    cubeObj.mesh.m_showSolid = true;  // Explicitly enable solid rendering
    cubeObj.mesh.m_showWireframe = true; // Explicitly enable wireframe
    cubeObj.name = "Cube 1";
    cubeObj.id = m_nextObjectId++;
    m_sceneObjects.push_back(cubeObj);
    m_renderer->loadMesh(cubeObj.mesh);
    
    // Create pyramid
    SceneObject pyramidObj;
    pyramidObj.mesh = MeshGenerator::createPyramid(1.5f);
    pyramidObj.mesh.setColor(0.0f, 1.0f, 0.0f);
    pyramidObj.mesh.setPosition(2.0f, 0.0f, 0.0f);
    pyramidObj.mesh.m_showSolid = true;  // Explicitly enable solid rendering
    pyramidObj.mesh.m_showWireframe = true; // Explicitly enable wireframe
    pyramidObj.name = "Pyramid 1";
    pyramidObj.id = m_nextObjectId++;
    m_sceneObjects.push_back(pyramidObj);
    m_renderer->loadMesh(pyramidObj.mesh);
    
    // Create another cube
    SceneObject cube2Obj;
    cube2Obj.mesh = MeshGenerator::createCube(0.8f);
    cube2Obj.mesh.setColor(0.0f, 0.0f, 1.0f);
    cube2Obj.mesh.setPosition(0.0f, 0.5f, 2.0f);
    cube2Obj.mesh.m_showSolid = true;  // Explicitly enable solid rendering
    cube2Obj.mesh.m_showWireframe = true; // Explicitly enable wireframe
    cube2Obj.name = "Cube 2";
    cube2Obj.id = m_nextObjectId++;
    m_sceneObjects.push_back(cube2Obj);
    m_renderer->loadMesh(cube2Obj.mesh);
}

void Application::setGlobalWireframeMode(bool enabled) {
    m_globalWireframeMode = enabled;
    for (auto& obj : m_sceneObjects) {
        obj.mesh.m_showWireframe = enabled;
    }
    syncMeshesToRenderer();
}

void Application::setGlobalSolidMode(bool enabled) {
    m_globalSolidMode = enabled;
    for (auto& obj : m_sceneObjects) {
        obj.mesh.m_showSolid = enabled;
    }
    syncMeshesToRenderer();
}

void Application::toggleWireframeMode() {
    setGlobalWireframeMode(!m_globalWireframeMode);
}

void Application::toggleSolidMode() {
    setGlobalSolidMode(!m_globalSolidMode);
}

void Application::addObject(const std::string& type) {
    SceneObject newObj;
    newObj.id = m_nextObjectId++;
    newObj.name = type + " " + std::to_string(newObj.id);
    
    if (type == "Cube") {
        newObj.mesh = MeshGenerator::createCube(1.0f);
        newObj.mesh.setColor(0.7f, 0.7f, 0.7f);
    } else if (type == "Pyramid") {
        newObj.mesh = MeshGenerator::createPyramid(1.0f);
        newObj.mesh.setColor(0.7f, 0.7f, 0.7f);
    }
    
    // Apply global rendering modes to new objects
    newObj.mesh.m_showWireframe = m_globalWireframeMode;
    newObj.mesh.m_showSolid = m_globalSolidMode;
    
    m_sceneObjects.push_back(newObj);
    m_renderer->loadMesh(newObj.mesh);
}

void Application::removeObject(int objectId) {
    auto it = std::find_if(m_sceneObjects.begin(), m_sceneObjects.end(),
        [objectId](const SceneObject& obj) { return obj.id == objectId; });
    
    if (it != m_sceneObjects.end()) {
        m_sceneObjects.erase(it);
        if (m_selectedObjectId == objectId) {
            m_selectedObjectId = -1;
        }
        // Note: In a complete implementation, we'd need to rebuild the renderer's mesh list
    }
}

void Application::selectObject(int objectId) {
    // Deselect all objects first
    for (auto& obj : m_sceneObjects) {
        obj.mesh.setSelected(false);
    }
    
    m_selectedObjectId = objectId;
    
    // Select the new object
    SceneObject* selectedObj = getSelectedObject();
    if (selectedObj) {
        selectedObj->mesh.setSelected(true);
    }
    
    syncMeshesToRenderer();
}

SceneObject* Application::getSelectedObject() {
    auto it = std::find_if(m_sceneObjects.begin(), m_sceneObjects.end(),
        [this](const SceneObject& obj) { return obj.id == m_selectedObjectId; });
    
    return (it != m_sceneObjects.end()) ? &(*it) : nullptr;
}

void Application::renderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {
                m_sceneObjects.clear();
                m_selectedObjectId = -1;
            }
            if (ImGui::MenuItem("Exit")) {
                m_running = false;
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem("Cube")) {
                addObject("Cube");
            }
            if (ImGui::MenuItem("Pyramid")) {
                addObject("Pyramid");
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
                toggleWireframeMode();
            }
            if (ImGui::MenuItem("Solid Mode", "S", m_globalSolidMode)) {
                toggleSolidMode();
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Wireframe Only", nullptr, m_globalWireframeMode && !m_globalSolidMode)) {
                setGlobalWireframeMode(true);
                setGlobalSolidMode(false);
            }
            if (ImGui::MenuItem("Solid Only", nullptr, !m_globalWireframeMode && m_globalSolidMode)) {
                setGlobalWireframeMode(false);
                setGlobalSolidMode(true);
            }
            if (ImGui::MenuItem("Wireframe + Solid", nullptr, m_globalWireframeMode && m_globalSolidMode)) {
                setGlobalWireframeMode(true);
                setGlobalSolidMode(true);
            }
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Application::renderObjectList() {
    if (!m_showObjectList) return;
    
    ImGui::Begin("Scene Objects", &m_showObjectList);
    
    for (auto& obj : m_sceneObjects) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
        if (obj.id == m_selectedObjectId) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        
        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)obj.id, flags, "%s", obj.name.c_str());
        
        if (ImGui::IsItemClicked()) {
            selectObject(obj.id);
        }
        
        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                removeObject(obj.id);
            }
            ImGui::EndPopup();
        }
        
        if (nodeOpen) {
            ImGui::TreePop();
        }
    }
    
    ImGui::End();
}

void Application::syncMeshesToRenderer() {
    std::vector<Mesh> meshes;
    for (const auto& obj : m_sceneObjects) {
        meshes.push_back(obj.mesh);
    }
    m_renderer->updateAllMeshes(meshes);
}

void Application::syncSelectedObjectMesh() {
    SceneObject* selectedObj = getSelectedObject();
    if (selectedObj) {
        // Find the index of the selected object
        for (size_t i = 0; i < m_sceneObjects.size(); ++i) {
            if (m_sceneObjects[i].id == selectedObj->id) {
                m_renderer->updateMesh(i, selectedObj->mesh);
                break;
            }
        }
    }
}

void Application::renderObjectProperties() {
    if (!m_showObjectProperties) return;
    
    ImGui::Begin("Properties", &m_showObjectProperties);
    
    SceneObject* selectedObj = getSelectedObject();
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
        
        // Sync changes to renderer immediately
        if (meshChanged) {
            syncSelectedObjectMesh();
        }
    } else {
        ImGui::Text("No object selected");
    }
    
    ImGui::End();
}

void Application::renderViewportControls() {
    ImGui::Begin("Viewport Controls");
    
    ImGui::Text("Camera Controls:");
    ImGui::Text("Left Mouse: Rotate");
    ImGui::Text("Right Mouse: Pan");
    ImGui::Text("Scroll: Zoom");
    
    ImGui::Separator();
    
    // Rendering mode controls
    ImGui::Text("Rendering Mode:");
    
    if (ImGui::Button("Wireframe Only")) {
        setGlobalWireframeMode(true);
        setGlobalSolidMode(false);
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Solid Only")) {
        setGlobalWireframeMode(false);
        setGlobalSolidMode(true);
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Both")) {
        setGlobalWireframeMode(true);
        setGlobalSolidMode(true);
    }
    
    ImGui::Separator();
    
    // Individual toggles
    if (ImGui::Checkbox("Show Wireframe", &m_globalWireframeMode)) {
        setGlobalWireframeMode(m_globalWireframeMode);
    }
    
    if (ImGui::Checkbox("Show Solid", &m_globalSolidMode)) {
        setGlobalSolidMode(m_globalSolidMode);
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Reset Camera")) {
        m_camera = std::make_unique<Camera>(1200.0f / 800.0f);
    }
    
    ImGui::End();
}

void Application::handleMouseInput() {
    // Check if ImGui wants to capture mouse input
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        // Reset mouse delta to prevent camera jumping when ImGui releases mouse
        double dummyX, dummyY;
        m_window->getMouseDelta(dummyX, dummyY);
        return; // ImGui is handling mouse input, don't process camera controls
    }
    
    double deltaX, deltaY;
    m_window->getMouseDelta(deltaX, deltaY);
    
    // Handle mouse clicks for object selection
    static bool leftButtonWasPressed = false;
    bool leftButtonPressed = m_window->isMouseButtonPressed(0);
    
    if (leftButtonPressed && !leftButtonWasPressed) {
        // Mouse button just pressed - record position
        double mouseX, mouseY;
        m_window->getMousePosition(mouseX, mouseY);
        handleMouseClick(mouseX, mouseY);
    } else if (leftButtonPressed) {
        // Mouse button held - rotate camera
        m_camera->rotate(deltaX * 0.5f, deltaY * 0.5f);
    }
    
    leftButtonWasPressed = leftButtonPressed;
    
    // Right mouse button - pan camera
    if (m_window->isMouseButtonPressed(1)) { // Right button
        m_camera->pan(deltaX, deltaY);
    }
    
    // Mouse wheel - zoom camera
    double scrollDelta = m_window->getScrollDelta();
    if (scrollDelta != 0.0) {
        m_camera->zoom(-scrollDelta * 0.1f); // Negative for natural zoom direction
    }
}

void Application::handleMouseClick(double x, double y) {
    // Perform raycast to find clicked object
    int clickedObjectIndex = performRaycast(x, y);
    
    if (clickedObjectIndex >= 0 && clickedObjectIndex < static_cast<int>(m_sceneObjects.size())) {
        selectObject(m_sceneObjects[clickedObjectIndex].id);
    } else {
        // Clicked on empty space - deselect all
        selectObject(-1);
    }
}

int Application::performRaycast(double mouseX, double mouseY) {
    // Convert screen coordinates to world ray
    glm::vec3 rayOrigin = m_camera->getPosition();
    glm::vec3 rayDir = screenToWorldRay(mouseX, mouseY);
    
    float closestDistance = std::numeric_limits<float>::max();
    int closestObjectIndex = -1;
    
    // Test ray against all objects
    for (size_t objIndex = 0; objIndex < m_sceneObjects.size(); ++objIndex) {
        const auto& obj = m_sceneObjects[objIndex];
        const auto& mesh = obj.mesh;
        
        // Transform ray to object space
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(mesh.m_position[0], mesh.m_position[1], mesh.m_position[2]));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(mesh.m_rotation[0]), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(mesh.m_rotation[1]), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(mesh.m_rotation[2]), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(mesh.m_scale[0], mesh.m_scale[1], mesh.m_scale[2]));
        
        glm::mat4 invModelMatrix = glm::inverse(modelMatrix);
        glm::vec3 localRayOrigin = glm::vec3(invModelMatrix * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localRayDir = glm::normalize(glm::vec3(invModelMatrix * glm::vec4(rayDir, 0.0f)));
        
        // Test ray against all triangles in the mesh
        for (const auto& triangle : mesh.m_triangles) {
            const auto& v0 = mesh.m_vertices[triangle.indices[0]].position;
            const auto& v1 = mesh.m_vertices[triangle.indices[1]].position;
            const auto& v2 = mesh.m_vertices[triangle.indices[2]].position;
            
            glm::vec3 vert0(v0[0], v0[1], v0[2]);
            glm::vec3 vert1(v1[0], v1[1], v1[2]);
            glm::vec3 vert2(v2[0], v2[1], v2[2]);
            
            float distance;
            if (rayIntersectsTriangle(localRayOrigin, localRayDir, vert0, vert1, vert2, distance)) {
                if (distance < closestDistance && distance > 0.0f) {
                    closestDistance = distance;
                    closestObjectIndex = static_cast<int>(objIndex);
                }
            }
        }
    }
    
    return closestObjectIndex;
}

bool Application::rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                                       const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                       float& distance) {
    const float EPSILON = 0.0000001f;
    
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON) {
        return false; // Ray is parallel to triangle
    }
    
    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);
    
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    
    float t = f * glm::dot(edge2, q);
    
    if (t > EPSILON) {
        distance = t;
        return true;
    }
    
    return false;
}

glm::vec3 Application::screenToWorldRay(double mouseX, double mouseY) {
    // Get viewport dimensions
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // Convert mouse coordinates to normalized device coordinates
    float x = (2.0f * mouseX) / viewport[2] - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / viewport[3];
    
    // Get projection and view matrices
    glm::mat4 projMatrix = m_camera->getProjectionMatrix();
    glm::mat4 viewMatrix = m_camera->getViewMatrix();
    
    // Calculate ray direction in world space
    glm::mat4 invProjView = glm::inverse(projMatrix * viewMatrix);
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 rayWorld = glm::inverse(viewMatrix) * rayEye;
    
    return glm::normalize(glm::vec3(rayWorld));
}
