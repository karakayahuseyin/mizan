#include "Viewport.h"
#include "../Application.h"
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glu.h>
#include <imgui.h>
#include <limits>

Viewport::Viewport(Window* window) 
    : m_window(window), m_renderer(nullptr), m_camera(nullptr) {
}

Viewport::~Viewport() {
    cleanup();
}

bool Viewport::initialize() {
    // Initialize camera with default aspect ratio
    m_camera = std::make_unique<Camera>(1200.0f / 800.0f);
    
    // Initialize renderer
    m_renderer = std::make_unique<Renderer>();
    m_renderer->initialize();
    
    return true;
}

void Viewport::render() {
    if (!m_renderer || !m_camera) return;
    
    // Handle mouse input
    handleMouseInput();
    
    // Setup 3D view with camera
    auto projMatrix = m_camera->getProjectionMatrix();
    auto viewMatrix = m_camera->getViewMatrix();
    
    // Pass matrices to renderer
    m_renderer->setProjectionMatrix(projMatrix);
    m_renderer->setViewMatrix(viewMatrix);
    m_renderer->setCameraPosition(m_camera->getPosition());
    
    // Render grid
    m_renderer->enableGrid(m_showGrid);
    
    // Render 3D content
    m_renderer->render();
}

void Viewport::cleanup() {
    if (m_renderer) {
        m_renderer->cleanup();
    }
    m_camera.reset();
}

void Viewport::handleMouseInput() {
    if (!m_window) return;
    
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

void Viewport::resetCamera() {
    if (m_window) {
        m_camera = std::make_unique<Camera>(static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()));
    } else {
        m_camera = std::make_unique<Camera>(1200.0f / 800.0f);
    }
}

void Viewport::resize(int width, int height) {
    if (m_camera) {
        m_camera->resize(width, height);
    }
    if (m_renderer) {
        m_renderer->resize(width, height);
        m_renderer->setProjectionMatrix(m_camera->getProjectionMatrix());
    }
}

void Viewport::loadMesh(const Mesh& mesh) {
    if (m_renderer) {
        m_renderer->loadMesh(mesh);
    }
}

void Viewport::updateMesh(size_t index, const Mesh& mesh) {
    if (m_renderer) {
        m_renderer->updateMesh(index, mesh);
    }
}

void Viewport::updateAllMeshes(const std::vector<Mesh>& meshes) {
    if (m_renderer) {
        m_renderer->updateAllMeshes(meshes);
    }
}

void Viewport::handleMouseClick(double x, double y) {
    // Perform raycast to find clicked object
    int clickedObjectIndex = performRaycast(x, y);
    
    if (clickedObjectIndex >= 0 && m_onObjectSelected) {
        // Get the actual object ID from the scene objects
        if (m_getSceneObjects) {
            const auto& sceneObjects = m_getSceneObjects();
            if (clickedObjectIndex < static_cast<int>(sceneObjects.size())) {
                m_onObjectSelected(sceneObjects[clickedObjectIndex].id);
                return;
            }
        }
    }
    
    // Clicked on empty space - deselect all
    if (m_onObjectSelected) {
        m_onObjectSelected(-1);
    }
}

int Viewport::performRaycast(double mouseX, double mouseY) {
    if (!m_camera || !m_getSceneObjects) return -1;
    
    // Convert screen coordinates to world ray
    glm::vec3 rayOrigin = m_camera->getPosition();
    glm::vec3 rayDir = screenToWorldRay(mouseX, mouseY);
    
    float closestDistance = std::numeric_limits<float>::max();
    int closestObjectIndex = -1;
    
    const auto& sceneObjects = m_getSceneObjects();
    
    // Test ray against all objects
    for (size_t objIndex = 0; objIndex < sceneObjects.size(); ++objIndex) {
        const auto& obj = sceneObjects[objIndex];
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

bool Viewport::rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
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

glm::vec3 Viewport::screenToWorldRay(double mouseX, double mouseY) {
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
