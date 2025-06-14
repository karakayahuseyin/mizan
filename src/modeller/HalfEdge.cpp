#include "HalfEdge.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include <glm/glm.hpp>

namespace BREP {

uint32_t HalfEdge::s_nextId = 1;

HalfEdge::HalfEdge(VertexPtr origin) 
    : m_id(s_nextId++), m_origin(origin) {
    // Don't add to vertex's half-edge list in constructor
    // This should be done after the HalfEdge is fully constructed and managed by shared_ptr
}

VertexPtr HalfEdge::getDestination() const {
    if (m_twin) {
        return m_twin->getOrigin();
    }
    return nullptr;
}

void HalfEdge::setOrigin(VertexPtr origin) {
    if (m_origin) {
        // Don't remove from vertex's half-edge list to avoid shared_ptr issues
        // m_origin->removeOutgoingHalfEdge(std::shared_ptr<HalfEdge>(this));
    }
    m_origin = origin;
    if (origin) {
        // Don't add to vertex's half-edge list to avoid shared_ptr issues
        // origin->addOutgoingHalfEdge(std::shared_ptr<HalfEdge>(this));
    }
}

void HalfEdge::setTwin(HalfEdgePtr twin) {
    m_twin = twin;
    // Remove the problematic reciprocal assignment to avoid circular shared_ptr issues
    // The twin relationship should be set up externally after both half-edges are created
}

void HalfEdge::setNext(HalfEdgePtr next) {
    m_next = next;
    // Don't set reciprocal relationship to avoid shared_ptr issues
    // if (next && next->getPrev().get() != this) {
    //     next->m_prev = std::shared_ptr<HalfEdge>(this);
    // }
}

void HalfEdge::setPrev(HalfEdgePtr prev) {
    m_prev = prev;
    // Don't set reciprocal relationship to avoid shared_ptr issues
    // if (prev && prev->getNext().get() != this) {
    //     prev->m_next = std::shared_ptr<HalfEdge>(this);
    // }
}

void HalfEdge::setEdge(EdgePtr edge) {
    m_edge = edge;
}

void HalfEdge::setFace(FacePtr face) {
    m_face = face;
}

glm::vec3 HalfEdge::getVector() const {
    if (m_origin && getDestination()) {
        return getDestination()->getPosition() - m_origin->getPosition();
    }
    return glm::vec3(0.0f);
}

float HalfEdge::getLength() const {
    return glm::length(getVector());
}

glm::vec3 HalfEdge::getMidpoint() const {
    if (m_origin && getDestination()) {
        return (m_origin->getPosition() + getDestination()->getPosition()) * 0.5f;
    }
    return glm::vec3(0.0f);
}

HalfEdgePtr HalfEdge::getNextAroundOrigin() const {
    if (m_twin && m_twin->getNext()) {
        return m_twin->getNext();
    }
    return nullptr;
}

HalfEdgePtr HalfEdge::getPrevAroundOrigin() const {
    if (m_prev && m_prev->getTwin()) {
        return m_prev->getTwin();
    }
    return nullptr;
}

HalfEdgePtr HalfEdge::getNextAroundDestination() const {
    if (m_next && m_next->getTwin()) {
        return m_next->getTwin();
    }
    return nullptr;
}

HalfEdgePtr HalfEdge::getPrevAroundDestination() const {
    if (m_twin && m_twin->getPrev()) {
        return m_twin->getPrev();
    }
    return nullptr;
}

bool HalfEdge::isValid() const {
    // Basic validity checks
    if (!m_origin) return false;
    
    // Check twin relationship
    if (m_twin) {
        if (m_twin->getTwin().get() != this) return false;
        if (!m_twin->getOrigin()) return false;
    }
    
    // Check next/prev relationship
    if (m_next && m_next->getPrev().get() != this) return false;
    if (m_prev && m_prev->getNext().get() != this) return false;
    
    return true;
}

bool HalfEdge::isBoundary() const {
    return m_face == nullptr;
}

} // namespace BREP
