#include "Edge.h"
#include "HalfEdge.h"
#include "Vertex.h"
#include "Face.h"
#include <glm/glm.hpp>

namespace BREP {

uint32_t Edge::s_nextId = 1;

Edge::Edge(HalfEdgePtr he1, HalfEdgePtr he2) 
    : m_id(s_nextId++), m_halfEdge1(he1), m_halfEdge2(he2) {
    // Don't set edge reference in constructor to avoid shared_ptr issues
    // This should be done after the Edge is fully constructed and managed by shared_ptr
}

std::pair<HalfEdgePtr, HalfEdgePtr> Edge::getHalfEdges() const {
    return std::make_pair(m_halfEdge1, m_halfEdge2);
}

VertexPtr Edge::getVertex1() const {
    return m_halfEdge1 ? m_halfEdge1->getOrigin() : nullptr;
}

VertexPtr Edge::getVertex2() const {
    return m_halfEdge1 ? m_halfEdge1->getDestination() : nullptr;
}

std::pair<VertexPtr, VertexPtr> Edge::getVertices() const {
    return std::make_pair(getVertex1(), getVertex2());
}

FacePtr Edge::getFace1() const {
    return m_halfEdge1 ? m_halfEdge1->getFace() : nullptr;
}

FacePtr Edge::getFace2() const {
    return m_halfEdge2 ? m_halfEdge2->getFace() : nullptr;
}

std::pair<FacePtr, FacePtr> Edge::getFaces() const {
    return std::make_pair(getFace1(), getFace2());
}

glm::vec3 Edge::getVector() const {
    if (m_halfEdge1) {
        return m_halfEdge1->getVector();
    }
    return glm::vec3(0.0f);
}

float Edge::getLength() const {
    return glm::length(getVector());
}

glm::vec3 Edge::getMidpoint() const {
    if (m_halfEdge1) {
        return m_halfEdge1->getMidpoint();
    }
    return glm::vec3(0.0f);
}

glm::vec3 Edge::getDirection() const {
    glm::vec3 vec = getVector();
    float length = glm::length(vec);
    if (length > 0.0f) {
        return vec / length;
    }
    return glm::vec3(0.0f);
}

bool Edge::isBoundary() const {
    return (getFace1() == nullptr) || (getFace2() == nullptr);
}

bool Edge::isManifold() const {
    return (getFace1() != nullptr) && (getFace2() != nullptr);
}

bool Edge::containsVertex(VertexPtr vertex) const {
    return (getVertex1() == vertex) || (getVertex2() == vertex);
}

VertexPtr Edge::getOtherVertex(VertexPtr vertex) const {
    VertexPtr v1 = getVertex1();
    VertexPtr v2 = getVertex2();
    
    if (v1 == vertex) return v2;
    if (v2 == vertex) return v1;
    return nullptr;
}

FacePtr Edge::getOtherFace(FacePtr face) const {
    FacePtr f1 = getFace1();
    FacePtr f2 = getFace2();
    
    if (f1 == face) return f2;
    if (f2 == face) return f1;
    return nullptr;
}

HalfEdgePtr Edge::getHalfEdgeWithFace(FacePtr face) const {
    if (m_halfEdge1 && m_halfEdge1->getFace() == face) {
        return m_halfEdge1;
    }
    if (m_halfEdge2 && m_halfEdge2->getFace() == face) {
        return m_halfEdge2;
    }
    return nullptr;
}

bool Edge::isValid() const {
    // Check that we have at least one half-edge
    if (!m_halfEdge1 && !m_halfEdge2) return false;
    
    // Check that half-edges are twins of each other
    if (m_halfEdge1 && m_halfEdge2) {
        if (m_halfEdge1->getTwin() != m_halfEdge2 || 
            m_halfEdge2->getTwin() != m_halfEdge1) {
            return false;
        }
    }
    
    // Check that both half-edges reference this edge
    if (m_halfEdge1 && m_halfEdge1->getEdge().get() != this) return false;
    if (m_halfEdge2 && m_halfEdge2->getEdge().get() != this) return false;
    
    return true;
}

} // namespace BREP
