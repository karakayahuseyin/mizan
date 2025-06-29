#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "renderer/Mesh.h"
#include "brep/Solid.h"

#include <string>
#include <memory>

struct SceneObject {
    BREP::Solid solid;
    Mesh mesh;
    std::string name;
    int id;
    bool visible = true;
};

#endif // SCENE_OBJECT_H