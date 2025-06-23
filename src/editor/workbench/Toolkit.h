#ifndef TOOLKIT_H
#define TOOLKIT_H

#include "window/Window.h"
#include "modeller/Modeller.h"
#include "scene/Scene.h"
#include "brep/Solid.h"
#include "Settings.h"

#include <memory>
#include <string>

/**
 * Toolkit is a ui module that provides a set of tools and utilities
 * for the editor's workbench. It includes functionalities for modelling
 * For modelling operations using the mizan::Modeller instance.
 */
class Toolkit {
public:
    Toolkit(Window* window, Scene* scene = nullptr);
    ~Toolkit();

    void initialize();
    void update();
    void render();
    void cleanup();

private:
    Window* m_window; // Pointer to the main application window for drawing ImGui elements
    Scene* m_scene; // Pointer to the scene containing all objects for interaction
    mizan::Modeller* m_modeller; // Pointer to the Modeller instance for modelling operations

    void addSolid(std::string name, BREP::Solid::PrimitiveType type);
    void renderMenuBar();
    void renderToolPanel();
};

#endif // TOOLKIT_H