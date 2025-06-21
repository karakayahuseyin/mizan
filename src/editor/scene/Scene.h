#ifndef SCENE_H
#define SCENE_H

#include "SceneObject.h"
#include <vector>

class Scene {
public:
    Scene();
    ~Scene();

    // Object management
    void addSolid(const BREP::Solid& solid, const std::string& name);
    void addObject(const std::string& type);
    void removeObject(int objectId);
    void selectObject(int objectId);
    SceneObject* getSelectedObject();
    const std::vector<SceneObject>& getSceneObjects() const { return m_sceneObjects; }

private:
    std::vector<SceneObject> m_sceneObjects;
    int m_nextObjectId = 1;
    int m_selectedObjectId = -1;
};

#endif // SCENE_H