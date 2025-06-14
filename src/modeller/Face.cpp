#include "Face.h"
#include "Loop.h"
#include "HalfEdge.h"
#include "Edge.h"
#include "Vertex.h"
#include "Shell.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <unordered_set>
#include <limits>

namespace BREP {

uint32_t Face::s_nextId = 1;

Face::Face(LoopPtr outerLoop) 
    : m_id(s_nextId++), m_outerLoop(outerLoop) {
    if (outerLoop) {
        outerLoop->setFace(std::shared_ptr<Face>(this));
        outerLoop->setOuter(true);
    }
}

std::vector<LoopPtr> Face::getAllLoops() const {
    std::vector<LoopPtr> loops;
    if (m_outerLoop) {
        loops.push_back(m_outerLoop);
    }
    loops.insert(loops.end(), m_innerLoops.begin(), m_innerLoops.end());
    return loops;
}

void Face::setOuterLoop(LoopPtr loop) {
    if (m_outerLoop) {
        m_outerLoop->setFace(nullptr);
    }
    m_outerLoop = loop;
    if (loop) {
        loop->setFace(std::shared_ptr<Face>(this));
        loop->setOuter(true);
    }
}

void Face::addInnerLoop(LoopPtr loop) {
    if (loop) {
        loop->setFace(std::shared_ptr<Face>(this));
        loop->setOuter(false);
        m_innerLoops.push_back(loop);
    }
}

void Face::removeInnerLoop(LoopPtr loop) {
    auto it = std::find(m_innerLoops.begin(), m_innerLoops.end(), loop);
    if (it != m_innerLoops.end()) {
        if (loop) {
            loop->setFace(nullptr);
        }
        m_innerLoops.erase(it);
    }
}

std::vector<HalfEdgePtr> Face::getHalfEdges() const {
    std::vector<HalfEdgePtr> halfEdges;
    
    auto loops = getAllLoops();
    for (const auto& loop : loops) {
        auto loopHalfEdges = loop->getHalfEdges();
        halfEdges.insert(halfEdges.end(), loopHalfEdges.begin(), loopHalfEdges.end());
    }
    
    return halfEdges;
}

std::vector<EdgePtr> Face::getEdges() const {
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

std::vector<VertexPtr> Face::getVertices() const {
    std::vector<VertexPtr> vertices;
    std::unordered_set<VertexPtr> vertexSet;
    
    auto loops = getAllLoops();
    for (const auto& loop : loops) {
        auto loopVertices = loop->getVertices();
        for (const auto& vertex : loopVertices) {
            vertexSet.insert(vertex);
        }
    }
    
    vertices.reserve(vertexSet.size());
    for (const auto& vertex : vertexSet) {
        vertices.push_back(vertex);
    }
    
    return vertices;
}

std::vector<FacePtr> Face::getAdjacentFaces() const {
    std::vector<FacePtr> faces;
    std::unordered_set<FacePtr> faceSet;
    
    auto edges = getEdges();
    for (const auto& edge : edges) {
        auto edgeFaces = edge->getFaces();
        if (edgeFaces.first && edgeFaces.first.get() != this) {
            faceSet.insert(edgeFaces.first);
        }
        if (edgeFaces.second && edgeFaces.second.get() != this) {
            faceSet.insert(edgeFaces.second);
        }
    }
    
    faces.reserve(faceSet.size());
    for (const auto& face : faceSet) {
        faces.push_back(face);
    }
    
    return faces;
}

glm::vec3 Face::getNormal() const {
    if (m_outerLoop) {
        return m_outerLoop->getNormal();
    }
    return glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Face::getCentroid() const {
    auto vertices = getVertices();
    if (vertices.empty()) return glm::vec3(0.0f);
    
    glm::vec3 centroid(0.0f);
    for (const auto& vertex : vertices) {
        centroid += vertex->getPosition();
    }
    
    return centroid / static_cast<float>(vertices.size());
}

float Face::getArea() const {
    float totalArea = 0.0f;
    
    if (m_outerLoop) {
        totalArea += m_outerLoop->getArea();
    }
    
    // Subtract areas of holes
    for (const auto& innerLoop : m_innerLoops) {
        totalArea -= innerLoop->getArea();
    }
    
    return totalArea;
}

float Face::getPerimeter() const {
    float totalPerimeter = 0.0f;
    
    if (m_outerLoop) {
        totalPerimeter += m_outerLoop->getPerimeter();
    }
    
    for (const auto& innerLoop : m_innerLoops) {
        totalPerimeter += innerLoop->getPerimeter();
    }
    
    return totalPerimeter;
}

bool Face::isFlat() const {
    auto vertices = getVertices();
    if (vertices.size() < 4) return true; // Triangle or less is always flat
    
    // Check if all vertices are coplanar
    if (vertices.size() < 3) return true;
    
    glm::vec3 normal = getNormal();
    glm::vec3 reference = vertices[0]->getPosition();
    
    const float tolerance = 1e-6f;
    for (size_t i = 1; i < vertices.size(); ++i) {
        glm::vec3 vec = vertices[i]->getPosition() - reference;
        float distance = std::abs(glm::dot(vec, normal));
        if (distance > tolerance) {
            return false;
        }
    }
    
    return true;
}

bool Face::contains(const glm::vec3& point) const {
    // This is a simplified 2D point-in-polygon test
    // For a complete implementation, project to the face plane first
    
    if (!m_outerLoop) return false;
    
    auto vertices = m_outerLoop->getVertices();
    if (vertices.size() < 3) return false;
    
    // Ray casting algorithm (simplified for XY plane)
    int intersections = 0;
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& v1 = vertices[i]->getPosition();
        const auto& v2 = vertices[(i + 1) % vertices.size()]->getPosition();
        
        if (((v1.y > point.y) != (v2.y > point.y)) &&
            (point.x < (v2.x - v1.x) * (point.y - v1.y) / (v2.y - v1.y) + v1.x)) {
            intersections++;
        }
    }
    
    bool insideOuter = (intersections % 2) == 1;
    if (!insideOuter) return false;
    
    // Check if point is inside any holes
    for (const auto& innerLoop : m_innerLoops) {
        auto innerVertices = innerLoop->getVertices();
        int innerIntersections = 0;
        
        for (size_t i = 0; i < innerVertices.size(); ++i) {
            const auto& v1 = innerVertices[i]->getPosition();
            const auto& v2 = innerVertices[(i + 1) % innerVertices.size()]->getPosition();
            
            if (((v1.y > point.y) != (v2.y > point.y)) &&
                (point.x < (v2.x - v1.x) * (point.y - v1.y) / (v2.y - v1.y) + v1.x)) {
                innerIntersections++;
            }
        }
        
        if ((innerIntersections % 2) == 1) {
            return false; // Point is inside a hole
        }
    }
    
    return true;
}

float Face::distanceToPoint(const glm::vec3& point) const {
    // Distance from point to face plane
    glm::vec3 normal = getNormal();
    glm::vec3 centroid = getCentroid();
    glm::vec3 vec = point - centroid;
    return std::abs(glm::dot(vec, normal));
}

bool Face::isBoundary() const {
    auto edges = getEdges();
    for (const auto& edge : edges) {
        if (edge->isBoundary()) {
            return true;
        }
    }
    return false;
}

bool Face::isManifold() const {
    auto edges = getEdges();
    for (const auto& edge : edges) {
        if (!edge->isManifold()) {
            return false;
        }
    }
    return true;
}

bool Face::contains(HalfEdgePtr halfEdge) const {
    auto halfEdges = getHalfEdges();
    return std::find(halfEdges.begin(), halfEdges.end(), halfEdge) != halfEdges.end();
}

bool Face::contains(EdgePtr edge) const {
    auto edges = getEdges();
    return std::find(edges.begin(), edges.end(), edge) != edges.end();
}

bool Face::contains(VertexPtr vertex) const {
    auto vertices = getVertices();
    return std::find(vertices.begin(), vertices.end(), vertex) != vertices.end();
}

bool Face::isAdjacent(FacePtr other) const {
    if (!other || other.get() == this) return false;
    
    auto adjacentFaces = getAdjacentFaces();
    return std::find(adjacentFaces.begin(), adjacentFaces.end(), other) != adjacentFaces.end();
}

bool Face::isValid() const {
    // Check outer loop
    if (!m_outerLoop || !m_outerLoop->isValid()) return false;
    
    // Check inner loops
    for (const auto& innerLoop : m_innerLoops) {
        if (!innerLoop || !innerLoop->isValid()) return false;
    }
    
    // Check that all half-edges in loops reference this face
    auto halfEdges = getHalfEdges();
    for (const auto& he : halfEdges) {
        if (!he || he->getFace().get() != this) return false;
    }
    
    return true;
}

bool Face::hasValidOrientation() const {
    // Check that outer loop has correct orientation (counterclockwise)
    if (m_outerLoop && m_outerLoop->isClockwise()) {
        return false;
    }
    
    // Check that inner loops have correct orientation (clockwise)
    for (const auto& innerLoop : m_innerLoops) {
        if (innerLoop && !innerLoop->isClockwise()) {
            return false;
        }
    }
    
    return true;
}

} // namespace BREP
