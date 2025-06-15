#ifndef UI_VIEWPORT_H
#define UI_VIEWPORT_H

#include "../renderer/Renderer.h"
#include "../renderer/Camera.h"
#include "Window.h"
#include <memory>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

struct SceneObject;

class Viewport {
private:
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    Window* m_window; // Reference to the window
    
    // Viewport settings
    bool m_showGrid = true;
    
    // Mouse interaction state
    bool m_isRotating = false;
    bool m_isPanning = false;
    
    // Callbacks for object selection
    std::function<void(int)> m_onObjectSelected;
    std::function<const std::vector<SceneObject>&()> m_getSceneObjects;
    
public:
    Viewport(Window* window);
    ~Viewport();
    
    bool initialize();
    void render();
    void cleanup();
    
    // Resize handling
    void resize(int width, int height);
    
    // Camera controls
    void handleMouseInput();
    void resetCamera();
    
    // Rendering settings
    void enableGrid(bool enable) { m_showGrid = enable; }
    bool isGridEnabled() const { return m_showGrid; }
    
    // Mesh management
    void loadMesh(const Mesh& mesh);
    void updateMesh(size_t index, const Mesh& mesh);
    void updateAllMeshes(const std::vector<Mesh>& meshes);
    
    // Object selection
    void handleMouseClick(double x, double y);
    int performRaycast(double mouseX, double mouseY);
    
    // Callbacks
    void setObjectSelectedCallback(std::function<void(int)> callback) {
        m_onObjectSelected = callback;
    }
    void setGetSceneObjectsCallback(std::function<const std::vector<SceneObject>&()> callback) {
        m_getSceneObjects = callback;
    }
    
    // Getters
    Camera* getCamera() const { return m_camera.get(); }
    Renderer* getRenderer() const { return m_renderer.get(); }

private:
    bool rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                              const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                              float& distance);
    glm::vec3 screenToWorldRay(double mouseX, double mouseY);
};

#endif // UI_VIEWPORT_H
