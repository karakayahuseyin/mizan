#ifndef APPLICATION_H
#define APPLICATION_H

// Use the UI Window class to avoid conflicts
#include "ui/Window.h"
#include "renderer/Renderer.h"
#include "renderer/Tesselator.h"
#include "renderer/Camera.h"
#include "modeller/BREPBuilder.h"
#include <memory>
#include <vector>
#include <string>

struct SceneObject {
    Mesh mesh;
    std::string name;
    int id;
    bool visible = true;
};

class Application {
private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    
    std::vector<SceneObject> m_sceneObjects;
    Mesh m_gridMesh; // Add grid mesh as a member
    int m_nextObjectId = 1;
    int m_selectedObjectId = -1;
    
    bool m_running;
    
    // UI state
    float m_triangleColor[3] = {1.0f, 0.5f, 0.0f};
    float m_rotation = 0.0f;
    bool m_showGrid = true;
    bool m_showObjectList = true;
    bool m_showObjectProperties = true;
    
    // Global rendering modes
    bool m_globalWireframeMode = false;
    bool m_globalSolidMode = true;
    
    // Mouse state
    bool m_isRotating = false;
    bool m_isPanning = false;

    // Mouse interaction
    void handleMouseClick(double x, double y);
    int performRaycast(double mouseX, double mouseY);
    bool rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                              const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                              float& distance);
    glm::vec3 screenToWorldRay(double mouseX, double mouseY);

public:
    Application();
    ~Application();

    bool init();
    void run();
    void shutdown();

private:
    void update();
    void render();
    void createTestMesh();
    
    // Object management
    void addObject(const std::string& type);
    void removeObject(int objectId);
    void selectObject(int objectId);
    SceneObject* getSelectedObject();
    
    // Mesh synchronization
    void syncMeshesToRenderer();
    void syncSelectedObjectMesh();
    
    // Rendering mode management
    void setGlobalWireframeMode(bool enabled);
    void setGlobalSolidMode(bool enabled);
    void toggleWireframeMode();
    void toggleSolidMode();
    
    // UI rendering
    void renderMainMenuBar();
    void renderObjectList();
    void renderObjectProperties();
    void renderViewportControls();
    
    // Input handling
    void handleMouseInput();
};

#endif // APPLICATION_H