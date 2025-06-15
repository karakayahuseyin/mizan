#include "Application.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>
#include <algorithm>
#include <cstring>

Application::Application() 
    : m_window(nullptr), m_viewport(nullptr), m_uiManager(nullptr), m_running(false), m_rotation(0.0f) {
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
    
    // Initialize viewport
    m_viewport = std::make_unique<Viewport>(m_window.get());
    if (!m_viewport->initialize()) {
        std::cerr << "Failed to initialize viewport" << std::endl;
        return false;
    }
    
    // Initialize UI manager
    m_uiManager = std::make_unique<UIManager>(m_window.get(), m_viewport.get());
    setupUICallbacks();
    
    // Setup viewport callbacks
    m_viewport->setObjectSelectedCallback([this](int objectId) {
        selectObject(objectId);
    });
    
    m_viewport->setGetSceneObjectsCallback([this]() -> const std::vector<SceneObject>& {
        return getSceneObjects();
    });
    
    // Setup window resize callback
    m_window->setResizeCallback([this](int width, int height) {
        if (m_viewport) {
            m_viewport->resize(width, height);
        }
    });
    
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
    
    // Render 3D viewport
    m_viewport->render();
    
    // Render UI
    m_uiManager->render();
}

void Application::shutdown() {
    if (m_uiManager) {
        m_uiManager.reset();
    }
    
    if (m_viewport) {
        m_viewport->cleanup();
        m_viewport.reset();
    }
    
    if (m_window) {
        m_window->cleanup();
        m_window.reset();
    }
    
    m_running = false;
}

void Application::createTestMesh() {
    // Create cube using BREP
    SceneObject cubeObj;
    BREP::Solid cubeSolid = BREP::BREPBuilder::createCubeSolid(1.0f);
    cubeObj.mesh = Tessellator::tessellate(cubeSolid);
    cubeObj.mesh.setColor(1.0f, 0.0f, 0.0f);
    cubeObj.mesh.setPosition(-3.0f, 1.0f, 0.0f);
    cubeObj.mesh.m_showSolid = m_globalSolidMode;
    cubeObj.mesh.m_showWireframe = m_globalWireframeMode;
    cubeObj.name = "Cube 1";
    cubeObj.id = m_nextObjectId++;
    m_sceneObjects.push_back(cubeObj);
    m_viewport->loadMesh(cubeObj.mesh);
    
    // Create pyramid using BREP
    SceneObject pyramidObj;
    BREP::Solid pyramidSolid = BREP::BREPBuilder::createPyramidSolid(1.5f);
    pyramidObj.mesh = Tessellator::tessellate(pyramidSolid);
    pyramidObj.mesh.setColor(0.0f, 1.0f, 0.0f);
    pyramidObj.mesh.setPosition(-1.0f, 0.0f, 0.0f);
    pyramidObj.mesh.m_showSolid = m_globalSolidMode;
    pyramidObj.mesh.m_showWireframe = m_globalWireframeMode;
    pyramidObj.name = "Pyramid 1";
    pyramidObj.id = m_nextObjectId++;
    m_sceneObjects.push_back(pyramidObj);
    m_viewport->loadMesh(pyramidObj.mesh);
    
    // Create sphere using BREP
    SceneObject sphereObj;
    BREP::Solid sphereSolid = BREP::BREPBuilder::createSphereSolid(0.7f, 12, 24);
    sphereObj.mesh = Tessellator::tessellate(sphereSolid);
    sphereObj.mesh.setColor(0.0f, 0.0f, 1.0f);
    sphereObj.mesh.setPosition(1.0f, 0.7f, 0.0f);
    sphereObj.mesh.m_showSolid = m_globalSolidMode;
    sphereObj.mesh.m_showWireframe = m_globalWireframeMode;
    sphereObj.name = "Sphere 1";
    sphereObj.id = m_nextObjectId++;
    m_sceneObjects.push_back(sphereObj);
    m_viewport->loadMesh(sphereObj.mesh);
    
    // Create cylinder using BREP
    SceneObject cylinderObj;
    BREP::Solid cylinderSolid = BREP::BREPBuilder::createCylinderSolid(0.5f, 1.2f, 16);
    cylinderObj.mesh = Tessellator::tessellate(cylinderSolid);
    cylinderObj.mesh.setColor(1.0f, 1.0f, 0.0f);
    cylinderObj.mesh.setPosition(3.0f, 0.6f, 0.0f);
    cylinderObj.mesh.m_showSolid = m_globalSolidMode;
    cylinderObj.mesh.m_showWireframe = m_globalWireframeMode;
    cylinderObj.name = "Cylinder 1";
    cylinderObj.id = m_nextObjectId++;
    m_sceneObjects.push_back(cylinderObj);
    m_viewport->loadMesh(cylinderObj.mesh);
    
    // Create another cube using BREP
    SceneObject cube2Obj;
    BREP::Solid cube2Solid = BREP::BREPBuilder::createCubeSolid(0.8f);
    cube2Obj.mesh = Tessellator::tessellate(cube2Solid);
    cube2Obj.mesh.setColor(1.0f, 0.0f, 1.0f);
    cube2Obj.mesh.setPosition(0.0f, 0.5f, 2.0f);
    cube2Obj.mesh.m_showSolid = m_globalSolidMode;
    cube2Obj.mesh.m_showWireframe = m_globalWireframeMode;
    cube2Obj.name = "Cube 2";
    cube2Obj.id = m_nextObjectId++;
    m_sceneObjects.push_back(cube2Obj);
    m_viewport->loadMesh(cube2Obj.mesh);
}

void Application::addTestObjects() {
    createTestMesh();
}

void Application::addObject(const std::string& type) {
    SceneObject newObj;
    newObj.id = m_nextObjectId++;
    newObj.name = type + " " + std::to_string(newObj.id);
    
    if (type == "Cube") {
        BREP::Solid cubeSolid = BREP::BREPBuilder::createCubeSolid(1.0f);
        newObj.mesh = Tessellator::tessellate(cubeSolid);
        newObj.mesh.setColor(0.7f, 0.7f, 0.7f);
    } else if (type == "Pyramid") {
        BREP::Solid pyramidSolid = BREP::BREPBuilder::createPyramidSolid(1.0f);
        newObj.mesh = Tessellator::tessellate(pyramidSolid);
        newObj.mesh.setColor(0.7f, 0.7f, 0.7f);
    } else if (type == "Sphere") {
        BREP::Solid sphereSolid = BREP::BREPBuilder::createSphereSolid(1.0f, 12, 24);
        newObj.mesh = Tessellator::tessellate(sphereSolid);
        newObj.mesh.setColor(0.7f, 0.7f, 0.7f);
    } else if (type == "Cylinder") {
        BREP::Solid cylinderSolid = BREP::BREPBuilder::createCylinderSolid(1.0f, 2.0f, 16);
        newObj.mesh = Tessellator::tessellate(cylinderSolid);
        newObj.mesh.setColor(0.7f, 0.7f, 0.7f);
    }
    
    // Apply global rendering modes to new objects
    newObj.mesh.m_showWireframe = m_globalWireframeMode;
    newObj.mesh.m_showSolid = m_globalSolidMode;
    
    m_sceneObjects.push_back(newObj);
    m_viewport->loadMesh(newObj.mesh);
}

void Application::setGlobalWireframeMode(bool enabled) {
    m_globalWireframeMode = enabled;
    if (m_uiManager) {
        m_uiManager->m_globalWireframeMode = enabled;
    }
    for (auto& obj : m_sceneObjects) {
        obj.mesh.m_showWireframe = enabled;
    }
    syncMeshesToViewport();
}

void Application::setGlobalSolidMode(bool enabled) {
    m_globalSolidMode = enabled;
    if (m_uiManager) {
        m_uiManager->m_globalSolidMode = enabled;
    }
    for (auto& obj : m_sceneObjects) {
        obj.mesh.m_showSolid = enabled;
    }
    syncMeshesToViewport();
}

void Application::toggleWireframeMode() {
    setGlobalWireframeMode(!m_globalWireframeMode);
}

void Application::toggleSolidMode() {
    setGlobalSolidMode(!m_globalSolidMode);
}

void Application::removeObject(int objectId) {
    auto it = std::find_if(m_sceneObjects.begin(), m_sceneObjects.end(),
        [objectId](const SceneObject& obj) { return obj.id == objectId; });
    
    if (it != m_sceneObjects.end()) {
        m_sceneObjects.erase(it);
        if (m_selectedObjectId == objectId) {
            m_selectedObjectId = -1;
        }
        // Note: In a complete implementation, we'd need to rebuild the viewport's mesh list
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
    
    syncMeshesToViewport();
}

SceneObject* Application::getSelectedObject() {
    auto it = std::find_if(m_sceneObjects.begin(), m_sceneObjects.end(),
        [this](const SceneObject& obj) { return obj.id == m_selectedObjectId; });
    
    return (it != m_sceneObjects.end()) ? &(*it) : nullptr;
}

void Application::setupUICallbacks() {
    m_uiManager->onExit = [this]() {
        m_running = false;
    };
    
    m_uiManager->onAddObject = [this](const std::string& type) {
        addObject(type);
    };
    
    m_uiManager->onRemoveObject = [this](int objectId) {
        removeObject(objectId);
    };
    
    m_uiManager->onSelectObject = [this](int objectId) {
        selectObject(objectId);
    };
    
    m_uiManager->onSetGlobalWireframeMode = [this](bool enabled) {
        setGlobalWireframeMode(enabled);
    };
    
    m_uiManager->onSetGlobalSolidMode = [this](bool enabled) {
        setGlobalSolidMode(enabled);
    };
    
    m_uiManager->onNewScene = [this]() {
        m_sceneObjects.clear();
        m_selectedObjectId = -1;
    };
    
    m_uiManager->onGetSelectedObject = [this]() -> SceneObject* {
        return getSelectedObject();
    };
    
    m_uiManager->onGetSceneObjects = [this]() -> const std::vector<SceneObject>& {
        return getSceneObjects();
    };
    
    m_uiManager->onSyncSelectedObjectMesh = [this]() {
        syncSelectedObjectMesh();
    };
}

void Application::syncMeshesToViewport() {
    std::vector<Mesh> meshes;
    for (const auto& obj : m_sceneObjects) {
        meshes.push_back(obj.mesh);
    }
    m_viewport->updateAllMeshes(meshes);
}

void Application::syncSelectedObjectMesh() {
    SceneObject* selectedObj = getSelectedObject();
    if (selectedObj) {
        // Find the index of the selected object
        for (size_t i = 0; i < m_sceneObjects.size(); ++i) {
            if (m_sceneObjects[i].id == selectedObj->id) {
                m_viewport->updateMesh(i, selectedObj->mesh);
                break;
            }
        }
    }
}
