#ifndef BREP_TYPES_H
#define BREP_TYPES_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace BREP {

// Forward declarations
class Vertex;
class Edge;
class Face;
class Solid;
class Shell;
class Loop;
class HalfEdge;

using VertexPtr = std::shared_ptr<Vertex>;
using EdgePtr = std::shared_ptr<Edge>;
using FacePtr = std::shared_ptr<Face>;
using SolidPtr = std::shared_ptr<Solid>;
using ShellPtr = std::shared_ptr<Shell>;
using LoopPtr = std::shared_ptr<Loop>;
using HalfEdgePtr = std::shared_ptr<HalfEdge>;

using VertexList = std::vector<VertexPtr>;
using EdgeList = std::vector<EdgePtr>;
using FaceList = std::vector<FacePtr>;
using SolidList = std::vector<SolidPtr>;
using ShellList = std::vector<ShellPtr>;
using LoopList = std::vector<LoopPtr>;
using HalfEdgeList = std::vector<HalfEdgePtr>;

} // namespace BREP

#endif // BREP_TYPES_H