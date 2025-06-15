#ifndef BREP_FACE_H
#define BREP_FACE_H

#include "BREPTypes.h"
#include <glm/glm.hpp>
#include <vector>

namespace BREP {

class Face {
private:
    static uint32_t s_nextId;
    uint32_t m_id;
    
    LoopPtr m_outerLoop;
    std::vector<LoopPtr> m_innerLoops; // Holes
    ShellPtr m_shell;

public:
    Face(LoopPtr outerLoop);
    ~Face() = default;

    // Getters
    uint32_t getId() const { return m_id; }
    LoopPtr getOuterLoop() const { return m_outerLoop; }
    const std::vector<LoopPtr>& getInnerLoops() const { return m_innerLoops; }
    std::vector<LoopPtr> getAllLoops() const;
    ShellPtr getShell() const { return m_shell; }

    // Setters
    void setOuterLoop(LoopPtr loop);
    void setShell(ShellPtr shell) { m_shell = shell; }

    // Loop management
    void addInnerLoop(LoopPtr loop);
    void removeInnerLoop(LoopPtr loop);
    bool hasInnerLoops() const { return !m_innerLoops.empty(); }
    size_t getInnerLoopCount() const { return m_innerLoops.size(); }

    // Topology queries
    std::vector<HalfEdgePtr> getHalfEdges() const;
    std::vector<EdgePtr> getEdges() const;
    std::vector<VertexPtr> getVertices() const;
    std::vector<FacePtr> getAdjacentFaces() const;
    
    // Geometric properties
    glm::vec3 getNormal() const;
    glm::vec3 getCentroid() const;
    float getArea() const;
    float getPerimeter() const;
    
    // Geometric queries
    bool isFlat() const; // Check if all vertices are coplanar
    bool contains(const glm::vec3& point) const; // Point-in-face test
    float distanceToPoint(const glm::vec3& point) const;
    
    // Topology queries
    bool isBoundary() const; // True if face is on the boundary of the solid
    bool isManifold() const;
    bool contains(HalfEdgePtr halfEdge) const;
    bool contains(EdgePtr edge) const;
    bool contains(VertexPtr vertex) const;
    bool isAdjacent(FacePtr other) const;
    
    // Validation
    bool isValid() const;
    bool hasValidOrientation() const;
};

} // namespace BREP

#endif // BREP_FACE_H