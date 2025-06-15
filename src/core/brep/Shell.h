#ifndef BREP_SHELL_H
#define BREP_SHELL_H

#include "BREPTypes.h"
#include <vector>
#include <unordered_set>

namespace BREP {

class Shell {
private:
    static uint32_t s_nextId;
    uint32_t m_id;
    
    std::vector<FacePtr> m_faces;
    SolidPtr m_solid;
    bool m_isOuter; // True for outer shell, false for inner shell (cavity)

public:
    Shell(bool isOuter = true);
    Shell(const std::vector<FacePtr>& faces, bool isOuter = true);
    ~Shell() = default;

    // Getters
    uint32_t getId() const { return m_id; }
    const std::vector<FacePtr>& getFaces() const { return m_faces; }
    SolidPtr getSolid() const { return m_solid; }
    bool isOuter() const { return m_isOuter; }
    bool isInner() const { return !m_isOuter; }

    // Setters
    void setSolid(SolidPtr solid) { m_solid = solid; }
    void setOuter(bool isOuter) { m_isOuter = isOuter; }

    // Face management
    void addFace(FacePtr face);
    void removeFace(FacePtr face);
    bool containsFace(FacePtr face) const;
    size_t getFaceCount() const { return m_faces.size(); }

    // Topology queries
    std::vector<EdgePtr> getEdges() const;
    std::vector<VertexPtr> getVertices() const;
    std::vector<EdgePtr> getBoundaryEdges() const; // Edges with only one face
    std::vector<VertexPtr> getBoundaryVertices() const;
    
    // Geometric properties
    glm::vec3 getCentroid() const;
    float getSurfaceArea() const;
    glm::vec3 getBoundingBoxMin() const;
    glm::vec3 getBoundingBoxMax() const;
    std::pair<glm::vec3, glm::vec3> getBoundingBox() const;

    // Topology validation
    bool isClosed() const; // No boundary edges
    bool isManifold() const; // Each edge connects exactly two faces
    bool isOriented() const; // Consistent face orientations
    bool isConnected() const; // All faces form a connected surface
    
    // Geometric queries
    bool isConvex() const;
    bool intersects(const Shell& other) const;
    bool contains(const glm::vec3& point) const; // Point inside shell test
    float distanceToPoint(const glm::vec3& point) const;
    
    // Operations
    void reverseOrientation(); // Flip all face normals
    Shell createReversed() const; // Create copy with reversed orientation
    bool orientConsistently(); // Try to make all faces consistently oriented
    
    // Validation
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Utilities
    void clear();
    bool isEmpty() const { return m_faces.empty(); }
    
    // Iterator support
    auto begin() { return m_faces.begin(); }
    auto end() { return m_faces.end(); }
    auto begin() const { return m_faces.begin(); }
    auto end() const { return m_faces.end(); }
};

} // namespace BREP

#endif // BREP_SHELL_H