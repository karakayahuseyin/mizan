#include "Vertex.h"
#include "HalfEdge.h"
#include "Edge.h"
#include "Face.h"
#include <algorithm>
#include <cmath>

namespace BREP {

uint32_t Vertex::s_nextId = 1;

Vertex::Vertex(const glm::vec3& position) 
    : m_id(s_nextId++), m_position(position) {
}

void Vertex::addOutgoingHalfEdge(HalfEdgePtr halfEdge) {
    if (halfEdge && halfEdge->getOrigin().get() == this) {
        m_outgoingHalfEdges.insert(halfEdge);
    }
}

void Vertex::removeOutgoingHalfEdge(HalfEdgePtr halfEdge) {
    m_outgoingHalfEdges.erase(halfEdge);
}

std::vector<EdgePtr> Vertex::getIncidentEdges() const {
    std::vector<EdgePtr> edges;
    std::unordered_set<EdgePtr> edgeSet;
    
    for (const auto& halfEdge : m_outgoingHalfEdges) {
        if (halfEdge && halfEdge->getEdge()) {
            edgeSet.insert(halfEdge->getEdge());
        }
    }
    
    edges.reserve(edgeSet.size());
    for (const auto& edge : edgeSet) {
        edges.push_back(edge);
    }
    
    return edges;
}

std::vector<FacePtr> Vertex::getIncidentFaces() const {
    std::vector<FacePtr> faces;
    std::unordered_set<FacePtr> faceSet;
    
    for (const auto& halfEdge : m_outgoingHalfEdges) {
        if (halfEdge && halfEdge->getFace()) {
            faceSet.insert(halfEdge->getFace());
        }
    }
    
    faces.reserve(faceSet.size());
    for (const auto& face : faceSet) {
        faces.push_back(face);
    }
    
    return faces;
}

int Vertex::getValence() const {
    return static_cast<int>(getIncidentEdges().size());
}

float Vertex::distanceTo(const Vertex& other) const {
    return glm::distance(m_position, other.m_position);
}

float Vertex::distanceTo(const glm::vec3& point) const {
    return glm::distance(m_position, point);
}

bool Vertex::isValid() const {
    // Check if all outgoing half-edges have this vertex as origin
    for (const auto& halfEdge : m_outgoingHalfEdges) {
        if (!halfEdge || halfEdge->getOrigin().get() != this) {
            return false;
        }
    }
    return true;
}

} // namespace BREP
