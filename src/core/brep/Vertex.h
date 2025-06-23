#ifndef BREP_VERTEX_H
#define BREP_VERTEX_H

#include "Types.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <unordered_set>

namespace BREP {

class Vertex {
private:
    static uint32_t s_nextId;
    uint32_t m_id;
    glm::vec3 m_position;
    std::vector<std::weak_ptr<HalfEdge>> m_outgoingHalfEdges;  // Use weak_ptr to break circular dependency

public:
    Vertex(const glm::vec3& position);
    ~Vertex() = default;

    // Getters
    uint32_t getId() const { return m_id; }
    const glm::vec3& getPosition() const { return m_position; }
    std::vector<HalfEdgePtr> getOutgoingHalfEdges() const;  // Convert weak_ptr to shared_ptr

    // Setters
    void setPosition(const glm::vec3& position) { m_position = position; }

    // Topology methods
    void addOutgoingHalfEdge(HalfEdgePtr halfEdge);
    void removeOutgoingHalfEdge(HalfEdgePtr halfEdge);
    
    // Utility methods
    std::vector<EdgePtr> getIncidentEdges() const;
    std::vector<FacePtr> getIncidentFaces() const;
    int getValence() const; // Number of incident edges
    
    // Geometric queries
    float distanceTo(const Vertex& other) const;
    float distanceTo(const glm::vec3& point) const;
    
    // Validation
    bool isValid() const;
};

} // namespace BREP

#endif // BREP_VERTEX_H