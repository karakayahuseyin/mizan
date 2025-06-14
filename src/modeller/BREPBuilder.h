#ifndef BREP_BUILDER_H
#define BREP_BUILDER_H

#include "Solid.h"

namespace BREP {

class BREPBuilder {
public:
    static Solid createCubeSolid(float size = 1.0f);
    static Solid createPyramidSolid(float size = 1.0f);
    static Solid createSphereSolid(float radius = 1.0f, int latitudeSegments = 16, int longitudeSegments = 32);
    static Solid createCylinderSolid(float radius = 1.0f, float height = 2.0f, int segments = 16);
    static Solid createGridSolid(int size = 10, float spacing = 1.0f);

private:
    // Helper methods to create proper BREP topology
    static LoopPtr createSimpleLoop(const std::vector<VertexPtr>& vertices);
    static void connectHalfEdges(const std::vector<HalfEdgePtr>& halfEdges);
};

} // namespace BREP

#endif // BREP_BUILDER_H
