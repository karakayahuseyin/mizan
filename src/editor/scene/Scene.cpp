#include "Scene.h"
#include "renderer/Tessellator.h"
#include "brep/Builder.h"
#include "logger/Logger.h"

Scene::Scene() {
    // Initialize scene with default objects or state if needed
}

Scene::~Scene() {
    // Cleanup scene objects if necessary
    m_sceneObjects.clear();
}

bool Scene::addObject(SceneObject& object) {
    // object.id = getNextObjectId();
    m_sceneObjects.push_back(object);
    m_onObjectAddedCallback(object);
    return true;
}