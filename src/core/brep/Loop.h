#ifndef BREP_LOOP_H
#define BREP_LOOP_H

#include "BREPTypes.h"
#include <vector>

namespace BREP {

class Loop {
private:
    static uint32_t s_nextId;
    uint32_t m_id;
    
    HalfEdgePtr m_startHalfEdge;
    FacePtr m_face;
    bool m_isOuter; // True for outer loop, false for inner loop (hole)

public:
    Loop(HalfEdgePtr startHalfEdge, bool isOuter = true);
    ~Loop() = default;

    // Getters
    uint32_t getId() const { return m_id; }
    HalfEdgePtr getStartHalfEdge() const { return m_startHalfEdge; }
    FacePtr getFace() const { return m_face; }
    bool isOuter() const { return m_isOuter; }
    bool isInner() const { return !m_isOuter; }

    // Setters
    void setStartHalfEdge(HalfEdgePtr halfEdge) { m_startHalfEdge = halfEdge; }
    void setFace(FacePtr face) { m_face = face; }
    void setOuter(bool isOuter) { m_isOuter = isOuter; }

    // Traversal methods
    std::vector<HalfEdgePtr> getHalfEdges() const;
    std::vector<VertexPtr> getVertices() const;
    std::vector<EdgePtr> getEdges() const;
    
    // Geometric properties
    int getSize() const; // Number of edges/vertices in loop
    glm::vec3 getCentroid() const;
    float getPerimeter() const;
    
    // Geometric queries
    bool isClockwise() const; // Check orientation (assumes planar loop)
    glm::vec3 getNormal() const; // Compute normal vector (assumes planar loop)
    float getArea() const; // Compute area (assumes planar loop)
    
    // Topology queries
    bool isClosed() const;
    bool contains(HalfEdgePtr halfEdge) const;
    bool contains(VertexPtr vertex) const;
    bool contains(EdgePtr edge) const;
    
    // Validation
    bool isValid() const;
    bool isManifold() const;
};

} // namespace BREP

#endif // BREP_LOOP_H