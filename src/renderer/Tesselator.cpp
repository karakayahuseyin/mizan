#include "Tesselator.h"
#include "../modeller/Face.h"
#include "../modeller/Loop.h"
#include "../modeller/Vertex.h"
#include <cmath>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Mesh::clear() {
    m_vertices.clear();
    m_triangles.clear();
}

Mesh Tessellator::tessellate(const BREP::Solid& solid) {
    Mesh mesh;
    
    for (const auto& shell : solid.getShells()) {
        processShell(shell, mesh);
    }
    
    return mesh;
}

void Tessellator::processShell(const std::shared_ptr<BREP::Shell>& shell, Mesh& mesh) {
    for (const auto& face : shell->getFaces()) {
        processFace(face, mesh);
    }
}

void Tessellator::processFace(const std::shared_ptr<BREP::Face>& face, Mesh& mesh) {
    const auto& outerLoop = face->getOuterLoop();
    const auto& vertices = outerLoop->getVertices();
    
    if (vertices.size() < 3) {
        return; // Not enough vertices for a face
    }
    
    // Calculate face normal using first three vertices with proper cross product
    std::array<float, 3> faceNormal = {0.0f, 0.0f, 1.0f};
    if (vertices.size() >= 3) {
        auto pos0 = vertices[0]->getPosition();
        auto pos1 = vertices[1]->getPosition();
        auto pos2 = vertices[2]->getPosition();
        
        auto v1 = pos1 - pos0;
        auto v2 = pos2 - pos0;
        
        // Cross product: v1 × v2
        glm::vec3 normal = glm::cross(v1, v2);
        
        // Normalize
        float length = glm::length(normal);
        if (length > 0.0f) {
            normal = normal / length;
            faceNormal[0] = normal.x;
            faceNormal[1] = normal.y;
            faceNormal[2] = normal.z;
        }
    }
    
    // Store starting index for this face
    unsigned int startIndex = static_cast<unsigned int>(mesh.m_vertices.size());
    
    // Add vertices to mesh
    for (const auto& vertex : vertices) {
        RenderVertex renderVertex;
        auto pos = vertex->getPosition();
        renderVertex.position = {{static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z)}};
        
        // For spherical objects, use position as normal (after normalization)
        // Check if this vertex is likely from a sphere by checking distance from origin
        float distFromOrigin = glm::length(pos);
        if (distFromOrigin > 0.01f) { // Not at origin
            // Check if all vertices are roughly equidistant from origin (sphere-like)
            bool isSpherelike = true;
            float firstDist = glm::length(vertices[0]->getPosition());
            for (const auto& v : vertices) {
                float dist = glm::length(v->getPosition());
                if (std::abs(dist - firstDist) > 0.1f) { // Allow some tolerance
                    isSpherelike = false;
                    break;
                }
            }
            
            if (isSpherelike && vertices.size() == 3) { // Spherical triangle
                glm::vec3 vertexNormal = glm::normalize(pos);
                renderVertex.normal = {{vertexNormal.x, vertexNormal.y, vertexNormal.z}};
            } else {
                renderVertex.normal = faceNormal;
            }
        } else {
            renderVertex.normal = faceNormal;
        }
        
        mesh.m_vertices.push_back(renderVertex);
    }
    
    // Triangulate the face (simple fan triangulation for convex faces)
    for (size_t i = 1; i < vertices.size() - 1; ++i) {
        Triangle triangle;
        triangle.indices = {{startIndex, startIndex + static_cast<unsigned int>(i), startIndex + static_cast<unsigned int>(i + 1)}};
        mesh.m_triangles.push_back(triangle);
    }
}

void Mesh::setColor(float r, float g, float b) {
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
}

void Mesh::setRotation(float x, float y, float z) {
    m_rotation[0] = x;
    m_rotation[1] = y;
    m_rotation[2] = z;
}

void Mesh::setPosition(float x, float y, float z) {
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void Mesh::setScale(float x, float y, float z) {
    m_scale[0] = x;
    m_scale[1] = y;
    m_scale[2] = z;
}

void Mesh::setWireframeColor(float r, float g, float b) {
    m_wireframeColor[0] = r;
    m_wireframeColor[1] = g;
    m_wireframeColor[2] = b;
}

void Mesh::setSelected(bool selected) {
    m_isSelected = selected;
}

std::array<float, 3> Mesh::getCurrentWireframeColor() const {
    return m_isSelected ? m_selectedWireframeColor : m_wireframeColor;
}

std::vector<std::pair<unsigned int, unsigned int>> Mesh::getEdges() const {
    std::vector<std::pair<unsigned int, unsigned int>> edges;
    std::set<std::pair<unsigned int, unsigned int>> edgeSet;
    
    // Extract unique edges from triangles
    for (const auto& triangle : m_triangles) {
        for (int i = 0; i < 3; ++i) {
            unsigned int v1 = triangle.indices[i];
            unsigned int v2 = triangle.indices[(i + 1) % 3];
            
            // Ensure consistent edge ordering (smaller index first)
            if (v1 > v2) std::swap(v1, v2);
            edgeSet.insert({v1, v2});
        }
    }
    
    // Convert set to vector
    edges.reserve(edgeSet.size());
    for (const auto& edge : edgeSet) {
        edges.push_back(edge);
    }
    
    return edges;
}
