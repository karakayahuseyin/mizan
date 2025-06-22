#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include "brep/Types.h"
#include "Mesh.h"

#include <memory>

class Tessellator {
public:
    static Mesh tessellate(const BREP::Solid& solid);
    
private:
    static void processShell(const std::shared_ptr<BREP::Shell>& shell, Mesh& mesh);
    static void processFace(const std::shared_ptr<BREP::Face>& face, Mesh& mesh);
};

#endif