#include "Shell.h"
#include "Face.h"
#include "Edge.h"
#include "Vertex.h"
#include "Solid.h"
#include "HalfEdge.h"
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <limits>

namespace BREP {

uint32_t Shell::s_nextId = 1;

Shell::Shell(bool isOuter) 
    : m_id(s_nextId++), m_isOuter(isOuter) {
}

Shell::Shell(const std::vector<FacePtr>& faces, bool isOuter) 
    : m_id(s_nextId++), m_faces(faces), m_isOuter(isOuter) {
    // Set shell reference for all faces
    for (auto& face : m_faces) {
        if (face) {
            face->setShell(std::shared_ptr<Shell>(this));
        }
    }
}

void Shell::addFace(FacePtr face) {
    if (face && !containsFace(face)) {
        m_faces.push_back(face);
        face->setShell(std::shared_ptr<Shell>(this));
    }
}

void Shell::removeFace(FacePtr face) {
    auto it = std::find(m_faces.begin(), m_faces.end(), face);
    if (it != m_faces.end()) {
        if (face) {
            face->setShell(nullptr);
        }
        m_faces.erase(it);
    }
}

bool Shell::containsFace(FacePtr face) const {
    return std::find(m_faces.begin(), m_faces.end(), face) != m_faces.end();
}

std::vector<EdgePtr> Shell::getEdges() const {
    std::vector<EdgePtr> edges;
    std::unordered_set<EdgePtr> edgeSet;
    
    for (const auto& face : m_faces) {
        if (face) {
            auto faceEdges = face->getEdges();
            for (const auto& edge : faceEdges) {
                edgeSet.insert(edge);
            }
        }
    }
    
    edges.reserve(edgeSet.size());
    for (const auto& edge : edgeSet) {
        edges.push_back(edge);
    }
    
    return edges;
}

std::vector<VertexPtr> Shell::getVertices() const {
    std::vector<VertexPtr> vertices;
    std::unordered_set<VertexPtr> vertexSet;
    
    for (const auto& face : m_faces) {
        if (face) {
            auto faceVertices = face->getVertices();
            for (const auto& vertex : faceVertices) {
                vertexSet.insert(vertex);
            }
        }
    }
    
    vertices.reserve(vertexSet.size());
    for (const auto& vertex : vertexSet) {
        vertices.push_back(vertex);
    }
    
    return vertices;
}

std::vector<EdgePtr> Shell::getBoundaryEdges() const {
    std::vector<EdgePtr> boundaryEdges;
    auto edges = getEdges();
    
    for (const auto& edge : edges) {
        if (edge && edge->isBoundary()) {
            boundaryEdges.push_back(edge);
        }
    }
    
    return boundaryEdges;
}

std::vector<VertexPtr> Shell::getBoundaryVertices() const {
    std::vector<VertexPtr> boundaryVertices;
    std::unordered_set<VertexPtr> vertexSet;
    
    auto boundaryEdges = getBoundaryEdges();
    for (const auto& edge : boundaryEdges) {
        if (edge) {
            auto vertices = edge->getVertices();
            vertexSet.insert(vertices.first);
            vertexSet.insert(vertices.second);
        }
    }
    
    boundaryVertices.reserve(vertexSet.size());
    for (const auto& vertex : vertexSet) {
        boundaryVertices.push_back(vertex);
    }
    
    return boundaryVertices;
}

glm::vec3 Shell::getCentroid() const {
    auto vertices = getVertices();
    if (vertices.empty()) return glm::vec3(0.0f);
    
    glm::vec3 centroid(0.0f);
    for (const auto& vertex : vertices) {
        centroid += vertex->getPosition();
    }
    
    return centroid / static_cast<float>(vertices.size());
}

float Shell::getSurfaceArea() const {
    float totalArea = 0.0f;
    for (const auto& face : m_faces) {
        if (face) {
            totalArea += face->getArea();
        }
    }
    return totalArea;
}

glm::vec3 Shell::getBoundingBoxMin() const {
    auto vertices = getVertices();
    if (vertices.empty()) return glm::vec3(0.0f);
    
    glm::vec3 minPoint = vertices[0]->getPosition();
    for (const auto& vertex : vertices) {
        const auto& pos = vertex->getPosition();
        minPoint.x = std::min(minPoint.x, pos.x);
        minPoint.y = std::min(minPoint.y, pos.y);
        minPoint.z = std::min(minPoint.z, pos.z);
    }
    
    return minPoint;
}

glm::vec3 Shell::getBoundingBoxMax() const {
    auto vertices = getVertices();
    if (vertices.empty()) return glm::vec3(0.0f);
    
    glm::vec3 maxPoint = vertices[0]->getPosition();
    for (const auto& vertex : vertices) {
        const auto& pos = vertex->getPosition();
        maxPoint.x = std::max(maxPoint.x, pos.x);
        maxPoint.y = std::max(maxPoint.y, pos.y);
        maxPoint.z = std::max(maxPoint.z, pos.z);
    }
    
    return maxPoint;
}

std::pair<glm::vec3, glm::vec3> Shell::getBoundingBox() const {
    return std::make_pair(getBoundingBoxMin(), getBoundingBoxMax());
}

bool Shell::isClosed() const {
    auto boundaryEdges = getBoundaryEdges();
    return boundaryEdges.empty();
}

bool Shell::isManifold() const {
    auto edges = getEdges();
    for (const auto& edge : edges) {
        if (!edge || !edge->isManifold()) {
            return false;
        }
    }
    return true;
}

bool Shell::isOriented() const {
    // Check if all faces have consistent orientation
    // This is a simplified check - a complete implementation would
    // traverse the face adjacency graph and check orientation consistency
    
    auto edges = getEdges();
    for (const auto& edge : edges) {
        if (!edge || edge->isBoundary()) continue;
        
        auto faces = edge->getFaces();
        if (!faces.first || !faces.second) continue;
        
        // Get half-edges from both faces for this edge
        auto he1 = edge->getHalfEdgeWithFace(faces.first);
        auto he2 = edge->getHalfEdgeWithFace(faces.second);
        
        if (!he1 || !he2) continue;
        
        // They should be twins (opposite orientation)
        if (he1->getTwin() != he2 || he2->getTwin() != he1) {
            return false;
        }
    }
    
    return true;
}

bool Shell::isConnected() const {
    if (m_faces.empty()) return true;
    if (m_faces.size() == 1) return true;
    
    // BFS to check if all faces are connected
    std::unordered_set<FacePtr> visited;
    std::queue<FacePtr> queue;
    
    queue.push(m_faces[0]);
    visited.insert(m_faces[0]);
    
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        
        auto adjacentFaces = current->getAdjacentFaces();
        for (const auto& adjacent : adjacentFaces) {
            if (containsFace(adjacent) && visited.find(adjacent) == visited.end()) {
                visited.insert(adjacent);
                queue.push(adjacent);
            }
        }
    }
    
    return visited.size() == m_faces.size();
}

bool Shell::isConvex() const {
    // A shell is convex if all face normals point outward
    // and no interior angle is > 180 degrees
    // This is a simplified implementation
    
    if (!isClosed() || !isManifold()) return false;
    
    glm::vec3 centroid = getCentroid();
    
    for (const auto& face : m_faces) {
        if (!face) continue;
        
        glm::vec3 faceCenter = face->getCentroid();
        glm::vec3 faceNormal = face->getNormal();
        glm::vec3 toCenter = centroid - faceCenter;
        
        // Normal should point away from centroid for convex shell
        if (glm::dot(faceNormal, toCenter) > 0.0f) {
            return false;
        }
    }
    
    return true;
}

bool Shell::intersects(const Shell& other) const {
    // Simplified intersection test using bounding boxes
    auto [minA, maxA] = getBoundingBox();
    auto [minB, maxB] = other.getBoundingBox();
    
    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}

bool Shell::contains(const glm::vec3& point) const {
    if (!isClosed()) return false;
    
    // Ray casting algorithm
    // Cast a ray from the point and count intersections with faces
    glm::vec3 rayDir(1.0f, 0.0f, 0.0f); // Ray along X-axis
    int intersectionCount = 0;
    
    for (const auto& face : m_faces) {
        if (!face) continue;
        
        // Simplified ray-face intersection
        // In a complete implementation, this would use proper ray-triangle intersection
        glm::vec3 faceCenter = face->getCentroid();
        glm::vec3 faceNormal = face->getNormal();
        
        // Check if ray intersects face plane
        float denom = glm::dot(rayDir, faceNormal);
        if (std::abs(denom) < 1e-6f) continue; // Ray parallel to face
        
        float t = glm::dot(faceCenter - point, faceNormal) / denom;
        if (t < 0.0f) continue; // Intersection behind ray origin
        
        glm::vec3 intersectionPoint = point + t * rayDir;
        
        // Check if intersection point is inside face (simplified)
        if (face->contains(intersectionPoint)) {
            intersectionCount++;
        }
    }
    
    return (intersectionCount % 2) == 1;
}

float Shell::distanceToPoint(const glm::vec3& point) const {
    float minDistance = std::numeric_limits<float>::max();
    
    for (const auto& face : m_faces) {
        if (face) {
            float distance = face->distanceToPoint(point);
            minDistance = std::min(minDistance, distance);
        }
    }
    
    return minDistance == std::numeric_limits<float>::max() ? 0.0f : minDistance;
}

void Shell::reverseOrientation() {
    for (auto& face : m_faces) {
        if (face) {
            // Reverse face orientation
            // This would involve reversing the order of vertices in loops
            // Implementation depends on how face orientation is stored
        }
    }
}

Shell Shell::createReversed() const {
    Shell reversed(m_isOuter);
    reversed.m_faces = m_faces; // Copy faces
    reversed.reverseOrientation();
    return reversed;
}

bool Shell::orientConsistently() {
    // Try to orient all faces consistently
    // This is a complex algorithm that would traverse face adjacency
    // and propagate orientation from a seed face
    
    if (m_faces.empty()) return true;
    
    // Mark faces as oriented
    std::unordered_set<FacePtr> oriented;
    std::queue<FacePtr> queue;
    
    // Start with first face
    queue.push(m_faces[0]);
    oriented.insert(m_faces[0]);
    
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        
        auto adjacentFaces = current->getAdjacentFaces();
        for (const auto& adjacent : adjacentFaces) {
            if (!containsFace(adjacent) || oriented.count(adjacent)) continue;
            
            // Check and potentially fix orientation relative to current
            // Implementation would compare half-edge orientations
            // and flip if necessary
            
            oriented.insert(adjacent);
            queue.push(adjacent);
        }
    }
    
    return oriented.size() == m_faces.size();
}

bool Shell::isValid() const {
    // Check basic validity
    for (const auto& face : m_faces) {
        if (!face || !face->isValid()) return false;
    }
    
    // Check that all faces reference this shell
    for (const auto& face : m_faces) {
        if (face->getShell().get() != this) return false;
    }
    
    return true;
}

std::vector<std::string> Shell::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (m_faces.empty()) {
        errors.push_back("Shell has no faces");
    }
    
    if (!isValid()) {
        errors.push_back("Basic validation failed");
    }
    
    if (!isClosed()) {
        errors.push_back("Shell is not closed (has boundary edges)");
    }
    
    if (!isManifold()) {
        errors.push_back("Shell is not manifold");
    }
    
    if (!isOriented()) {
        errors.push_back("Shell has inconsistent face orientations");
    }
    
    if (!isConnected()) {
        errors.push_back("Shell faces are not connected");
    }
    
    return errors;
}

void Shell::clear() {
    for (auto& face : m_faces) {
        if (face) {
            face->setShell(nullptr);
        }
    }
    m_faces.clear();
}

} // namespace BREP
