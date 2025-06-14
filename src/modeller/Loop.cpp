#include "Loop.h"
#include "HalfEdge.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <unordered_set>

namespace BREP {

uint32_t Loop::s_nextId = 1;

Loop::Loop(HalfEdgePtr startHalfEdge, bool isOuter) 
    : m_id(s_nextId++), m_startHalfEdge(startHalfEdge), m_isOuter(isOuter) {
}

std::vector<HalfEdgePtr> Loop::getHalfEdges() const {
    std::vector<HalfEdgePtr> halfEdges;
    if (!m_startHalfEdge) return halfEdges;
    
    HalfEdgePtr current = m_startHalfEdge;
    do {
        halfEdges.push_back(current);
        current = current->getNext();
    } while (current && current != m_startHalfEdge);
    
    return halfEdges;
}

std::vector<VertexPtr> Loop::getVertices() const {
    std::vector<VertexPtr> vertices;
    if (!m_startHalfEdge) return vertices;
    
    HalfEdgePtr current = m_startHalfEdge;
    do {
        vertices.push_back(current->getOrigin());
        current = current->getNext();
    } while (current && current != m_startHalfEdge);
    
    return vertices;
}

std::vector<EdgePtr> Loop::getEdges() const {
    std::vector<EdgePtr> edges;
    std::unordered_set<EdgePtr> edgeSet;
    
    auto halfEdges = getHalfEdges();
    for (const auto& he : halfEdges) {
        if (he && he->getEdge()) {
            edgeSet.insert(he->getEdge());
        }
    }
    
    edges.reserve(edgeSet.size());
    for (const auto& edge : edgeSet) {
        edges.push_back(edge);
    }
    
    return edges;
}

int Loop::getSize() const {
    return static_cast<int>(getHalfEdges().size());
}

glm::vec3 Loop::getCentroid() const {
    auto vertices = getVertices();
    if (vertices.empty()) return glm::vec3(0.0f);
    
    glm::vec3 centroid(0.0f);
    for (const auto& vertex : vertices) {
        centroid += vertex->getPosition();
    }
    
    return centroid / static_cast<float>(vertices.size());
}

float Loop::getPerimeter() const {
    float perimeter = 0.0f;
    auto halfEdges = getHalfEdges();
    
    for (const auto& he : halfEdges) {
        if (he) {
            perimeter += he->getLength();
        }
    }
    
    return perimeter;
}

bool Loop::isClockwise() const {
    auto vertices = getVertices();
    if (vertices.size() < 3) return false;
    
    // Calculate signed area to determine orientation
    float signedArea = 0.0f;
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& v1 = vertices[i]->getPosition();
        const auto& v2 = vertices[(i + 1) % vertices.size()]->getPosition();
        signedArea += (v2.x - v1.x) * (v2.y + v1.y);
    }
    
    return signedArea > 0.0f;
}

glm::vec3 Loop::getNormal() const {
    auto vertices = getVertices();
    if (vertices.size() < 3) return glm::vec3(0.0f);
    
    // Use Newell's method to compute normal
    glm::vec3 normal(0.0f);
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& v1 = vertices[i]->getPosition();
        const auto& v2 = vertices[(i + 1) % vertices.size()]->getPosition();
        
        normal.x += (v1.y - v2.y) * (v1.z + v2.z);
        normal.y += (v1.z - v2.z) * (v1.x + v2.x);
        normal.z += (v1.x - v2.x) * (v1.y + v2.y);
    }
    
    float length = glm::length(normal);
    if (length > 0.0f) {
        return normal / length;
    }
    
    return glm::vec3(0.0f, 0.0f, 1.0f); // Default to Z-up
}

float Loop::getArea() const {
    auto vertices = getVertices();
    if (vertices.size() < 3) return 0.0f;
    
    // Use shoelace formula for 2D projection, then scale by normal magnitude
    glm::vec3 normal = getNormal();
    
    // Project to the plane with largest normal component
    int maxComponent = 0;
    if (std::abs(normal.y) > std::abs(normal.x)) maxComponent = 1;
    if (std::abs(normal.z) > std::abs(normal[maxComponent])) maxComponent = 2;
    
    float area = 0.0f;
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& v1 = vertices[i]->getPosition();
        const auto& v2 = vertices[(i + 1) % vertices.size()]->getPosition();
        
        float x1, y1, x2, y2;
        if (maxComponent == 0) { // Project to YZ plane
            x1 = v1.y; y1 = v1.z;
            x2 = v2.y; y2 = v2.z;
        } else if (maxComponent == 1) { // Project to XZ plane
            x1 = v1.x; y1 = v1.z;
            x2 = v2.x; y2 = v2.z;
        } else { // Project to XY plane
            x1 = v1.x; y1 = v1.y;
            x2 = v2.x; y2 = v2.y;
        }
        
        area += x1 * y2 - x2 * y1;
    }
    
    return std::abs(area) * 0.5f;
}

bool Loop::isClosed() const {
    if (!m_startHalfEdge) return false;
    
    HalfEdgePtr current = m_startHalfEdge;
    do {
        if (!current->getNext()) return false;
        current = current->getNext();
    } while (current && current != m_startHalfEdge);
    
    return current == m_startHalfEdge;
}

bool Loop::contains(HalfEdgePtr halfEdge) const {
    auto halfEdges = getHalfEdges();
    return std::find(halfEdges.begin(), halfEdges.end(), halfEdge) != halfEdges.end();
}

bool Loop::contains(VertexPtr vertex) const {
    auto vertices = getVertices();
    return std::find(vertices.begin(), vertices.end(), vertex) != vertices.end();
}

bool Loop::contains(EdgePtr edge) const {
    auto edges = getEdges();
    return std::find(edges.begin(), edges.end(), edge) != edges.end();
}

bool Loop::isValid() const {
    if (!m_startHalfEdge) return false;
    
    // Check if loop is closed
    if (!isClosed()) return false;
    
    // Check if all half-edges in loop are valid
    auto halfEdges = getHalfEdges();
    for (const auto& he : halfEdges) {
        if (!he || !he->isValid()) return false;
    }
    
    return true;
}

bool Loop::isManifold() const {
    // A loop is manifold if each vertex appears exactly once
    auto vertices = getVertices();
    std::unordered_set<VertexPtr> vertexSet(vertices.begin(), vertices.end());
    return vertices.size() == vertexSet.size();
}

} // namespace BREP
