#ifndef UI_UIMANAGER_H
#define UI_UIMANAGER_H

#include <imgui.h>
#include <string>
#include <vector>
#include <functional>

// Forward declarations
class Window;
class Viewport;
struct SceneObject;

class UIManager {
public:
    UIManager(Window* window, Viewport* viewport);
    ~UIManager();

    void render();

    // Callbacks
    std::function<void()> onExit;
    std::function<void()> onNewScene;
    std::function<void(const std::string&)> onAddObject;
    std::function<void(int)> onSelectObject;
    std::function<void(int)> onRemoveObject;
    std::function<std::vector<SceneObject>()> onGetSceneObjects;
    std::function<SceneObject*()> onGetSelectedObject;
    std::function<void()> onSyncSelectedObjectMesh;
    std::function<void(bool)> onSetGlobalWireframeMode;
    std::function<void(bool)> onSetGlobalSolidMode;
    
    // Add accessor methods for global rendering modes
    void setGlobalWireframeMode(bool enabled) { m_globalWireframeMode = enabled; }
    void setGlobalSolidMode(bool enabled) { m_globalSolidMode = enabled; }
    bool getGlobalWireframeMode() const { return m_globalWireframeMode; }
    bool getGlobalSolidMode() const { return m_globalSolidMode; }

private:
    // UI Components
    void renderMainMenuBar();
    void renderObjectList();
    void renderObjectProperties();
    void renderViewportControls();
    void renderKeyboardShortcuts();
    void handleKeyboardShortcuts();
    
    Window* m_window;
    Viewport* m_viewport;
    
    // UI state
    bool m_showObjectList = true;
    bool m_showObjectProperties = true;
    bool m_showGrid = true;
    bool m_globalWireframeMode = true;
    bool m_globalSolidMode = true;
};

#endif // UI_UIMANAGER_H
