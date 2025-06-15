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
    void handleKeyboardShortcuts();
    
    // UI state
    bool m_showObjectList = true;
    bool m_showObjectProperties = true;
    bool m_showGrid = true;
    
    // Global rendering modes
    bool m_globalWireframeMode = true;
    bool m_globalSolidMode = true;

    // Callbacks
    std::function<void()> onExit;
    std::function<void(const std::string&)> onAddObject;
    std::function<void(int)> onRemoveObject;
    std::function<void(int)> onSelectObject;
    std::function<void(bool)> onSetGlobalWireframeMode;
    std::function<void(bool)> onSetGlobalSolidMode;
    std::function<void()> onNewScene;
    std::function<SceneObject*()> onGetSelectedObject;
    std::function<const std::vector<SceneObject>&()> onGetSceneObjects;
    std::function<void()> onSyncSelectedObjectMesh;

private:
    Window* m_window;
    Viewport* m_viewport;

    void renderMainMenuBar();
    void renderObjectList();
    void renderObjectProperties();
    void renderViewportControls();
    void renderKeyboardShortcuts();
};

#endif // UI_UIMANAGER_H
