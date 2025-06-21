#include "Scene.h"
#include "renderer/Tessellator.h"
#include "brep/BREPBuilder.h"
#include "logger/Logger.h"

Scene::Scene() {
    // Initialize scene with default objects or state if needed
}

Scene::~Scene() {
    // Cleanup scene objects if necessary
    m_sceneObjects.clear();
}

void Scene::addObject(const std::string& type) {
    Logger::log("Adding object of type: ", Logger::LogLevel::Info);
}

void Scene::addSolid(const BREP::Solid& solid, const std::string& name) {
    Logger::log("Adding solid: ", Logger::LogLevel::Info);
}