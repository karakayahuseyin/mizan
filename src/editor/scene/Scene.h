#ifndef SCENE_H
#define SCENE_H

#include "SceneObject.h"
#include <vector>
#include <functional>
#include <string>

class Scene {
public:
    Scene();
    ~Scene();

    // Object management
    void addObject(SceneObject& object);
    void removeObject(int objectId);
    void selectObject(int objectId);
    SceneObject* getSelectedObject();
    const std::vector<SceneObject>& getSceneObjects() const { return m_sceneObjects; }
    int getNextObjectId() { 
        return m_nextObjectId++; 
    }

    void setObjectAddedCallback(std::function<void(const SceneObject&)> callback) {
        m_onObjectAddedCallback = callback;
    }

private:
    std::vector<SceneObject> m_sceneObjects;
    int m_nextObjectId = 0;
    int m_selectedObjectId = -1;

    std::function<void(const SceneObject&)> m_onObjectAddedCallback;
};

#endif // SCENE_H