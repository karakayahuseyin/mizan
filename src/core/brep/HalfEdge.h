#ifndef BREP_HALFEDGE_H
#define BREP_HALFEDGE_H

#include "Types.h"
#include <memory>

namespace BREP {

class HalfEdge {
    friend class Builder; // Allow BREPBuilder to access private members
    
private:
    static uint32_t s_nextId;
    uint32_t m_id;
    
    VertexPtr m_origin;
    HalfEdgePtr m_twin;
    HalfEdgePtr m_next;
    std::weak_ptr<HalfEdge> m_prev;  // Use weak_ptr to break circular dependency
    EdgePtr m_edge;
    std::weak_ptr<Face> m_face;  // Use weak_ptr to break circular dependency

public:
    HalfEdge(VertexPtr origin);
    ~HalfEdge() = default;

    // Getters
    uint32_t getId() const { return m_id; }
    VertexPtr getOrigin() const { return m_origin; }
    VertexPtr getDestination() const;
    HalfEdgePtr getTwin() const { return m_twin; }
    HalfEdgePtr getNext() const { return m_next; }
    HalfEdgePtr getPrev() const { return m_prev.lock(); }  // Convert weak_ptr to shared_ptr
    EdgePtr getEdge() const { return m_edge; }
    FacePtr getFace() const { return m_face.lock(); }  // Convert weak_ptr to shared_ptr

    // Setters
    void setOrigin(VertexPtr origin);
    void setTwin(HalfEdgePtr twin);
    void setNext(HalfEdgePtr next);
    void setPrev(HalfEdgePtr prev);
    void setEdge(EdgePtr edge);
    void setFace(FacePtr face);  // weak_ptr assignment

    // Utility methods
    glm::vec3 getVector() const;
    float getLength() const;
    glm::vec3 getMidpoint() const;
    
    // Traversal methods
    HalfEdgePtr getNextAroundOrigin() const;
    HalfEdgePtr getPrevAroundOrigin() const;
    HalfEdgePtr getNextAroundDestination() const;
    HalfEdgePtr getPrevAroundDestination() const;
    
    // Validation
    bool isValid() const;
    bool isBoundary() const; // True if this half-edge is on the boundary (no face)
};

} // namespace BREP

#endif // BREP_HALFEDGE_H