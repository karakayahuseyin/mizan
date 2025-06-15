#ifndef APPLICATION_H
#define APPLICATION_H

// Use the UI Window class to avoid conflicts
#include "ui/Window.h"
#include "ui/Viewport.h"
#include "ui/UIManager.h"
#include "renderer/Tessellator.h"
#include "brep/BREPBuilder.h"
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct SceneObject {
    Mesh mesh;
    std::string name;
    int id;
    bool visible = true;
};

class Application {
private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Viewport> m_viewport;
    std::unique_ptr<UIManager> m_uiManager;
    
    std::vector<SceneObject> m_sceneObjects;
    int m_nextObjectId = 1;
    int m_selectedObjectId = -1;
    
    bool m_running;
    
    // Animation/update variables
    float m_rotation;
    float m_triangleColor[3];
    
    // Global rendering modes
    bool m_globalWireframeMode = false;
    bool m_globalSolidMode = true;

public:
    Application();
    ~Application();

    bool init();
    void run();
    void shutdown();
    
    // Public interface for main to create test objects
    void addTestObjects();

private:
    void update();
    void render();
    void createTestMesh();
    
    // Object management
    void addObject(const std::string& type);
    void removeObject(int objectId);
    void selectObject(int objectId);
    SceneObject* getSelectedObject();
    const std::vector<SceneObject>& getSceneObjects() const { return m_sceneObjects; }
    
    // Mesh synchronization
    void syncMeshesToViewport();
    void syncSelectedObjectMesh();
    
    // Rendering mode management
    void setGlobalWireframeMode(bool enabled);
    void setGlobalSolidMode(bool enabled);
    void toggleWireframeMode();
    void toggleSolidMode();
    
    void setupUICallbacks();
};

#endif // APPLICATION_H