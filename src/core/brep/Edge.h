#ifndef BREP_EDGE_H
#define BREP_EDGE_H

#include "BREPTypes.h"
#include <glm/glm.hpp>

namespace BREP {

class Edge {
private:
    static uint32_t s_nextId;
    uint32_t m_id;
    
    HalfEdgePtr m_halfEdge1;
    HalfEdgePtr m_halfEdge2;

public:
    Edge(HalfEdgePtr he1, HalfEdgePtr he2);
    ~Edge() = default;

    // Getters
    uint32_t getId() const { return m_id; }
    HalfEdgePtr getHalfEdge1() const { return m_halfEdge1; }
    HalfEdgePtr getHalfEdge2() const { return m_halfEdge2; }
    
    // Get the half-edge pairs
    std::pair<HalfEdgePtr, HalfEdgePtr> getHalfEdges() const;
    
    // Get vertices
    VertexPtr getVertex1() const;
    VertexPtr getVertex2() const;
    std::pair<VertexPtr, VertexPtr> getVertices() const;
    
    // Get faces (may be null for boundary edges)
    FacePtr getFace1() const;
    FacePtr getFace2() const;
    std::pair<FacePtr, FacePtr> getFaces() const;

    // Geometric properties
    glm::vec3 getVector() const;
    float getLength() const;
    glm::vec3 getMidpoint() const;
    glm::vec3 getDirection() const; // Normalized vector
    
    // Queries
    bool isBoundary() const; // True if edge is on boundary (has only one face)
    bool isManifold() const; // True if edge has exactly two faces
    bool containsVertex(VertexPtr vertex) const;
    VertexPtr getOtherVertex(VertexPtr vertex) const;
    FacePtr getOtherFace(FacePtr face) const;
    HalfEdgePtr getHalfEdgeWithFace(FacePtr face) const;
    
    // Validation
    bool isValid() const;
};

} // namespace BREP

#endif // BREP_EDGE_H