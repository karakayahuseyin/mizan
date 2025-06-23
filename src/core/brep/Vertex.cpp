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

std::vector<HalfEdgePtr> Vertex::getOutgoingHalfEdges() const {
    std::vector<HalfEdgePtr> result;
    for (const auto& weakHe : m_outgoingHalfEdges) {
        if (auto he = weakHe.lock()) {
            result.push_back(he);
        }
    }
    return result;
}

void Vertex::addOutgoingHalfEdge(HalfEdgePtr halfEdge) {
    if (halfEdge && halfEdge->getOrigin().get() == this) {
        // Check if already exists to avoid duplicates
        auto it = std::find_if(m_outgoingHalfEdges.begin(), m_outgoingHalfEdges.end(),
            [halfEdge](const std::weak_ptr<HalfEdge>& weak) {
                auto locked = weak.lock();
                return locked && locked == halfEdge;
            });
        
        if (it == m_outgoingHalfEdges.end()) {
            m_outgoingHalfEdges.push_back(halfEdge);
        }
    }
}

void Vertex::removeOutgoingHalfEdge(HalfEdgePtr halfEdge) {
    auto it = std::find_if(m_outgoingHalfEdges.begin(), m_outgoingHalfEdges.end(),
        [halfEdge](const std::weak_ptr<HalfEdge>& weak) {
            auto locked = weak.lock();
            return locked && locked == halfEdge;
        });
    
    if (it != m_outgoingHalfEdges.end()) {
        m_outgoingHalfEdges.erase(it);
    }
}

std::vector<EdgePtr> Vertex::getIncidentEdges() const {
    std::vector<EdgePtr> edges;
    std::unordered_set<EdgePtr> edgeSet;
    
    auto outgoingHalfEdges = getOutgoingHalfEdges();
    for (const auto& halfEdge : outgoingHalfEdges) {
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
    
    auto outgoingHalfEdges = getOutgoingHalfEdges();
    for (const auto& halfEdge : outgoingHalfEdges) {
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
    auto outgoingHalfEdges = getOutgoingHalfEdges();
    for (const auto& halfEdge : outgoingHalfEdges) {
        if (!halfEdge || halfEdge->getOrigin().get() != this) {
            return false;
        }
    }
    return true;
}

} // namespace BREP
