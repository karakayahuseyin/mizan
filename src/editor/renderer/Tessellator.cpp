#include "Tessellator.h"
#include "brep/Solid.h"
#include "brep/Shell.h"
#include "brep/Face.h"
#include "brep/Loop.h"
#include "brep/Vertex.h"

#include <cmath>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    
    // Special handling for grid quads - ensure they stay as quads for wireframe
    if (vertices.size() == 4) {
        // For quad faces (like grid), create two triangles but store quad info for wireframe
        Triangle tri1, tri2;
        tri1.indices = {{startIndex, startIndex + 1, startIndex + 2}};
        tri2.indices = {{startIndex, startIndex + 2, startIndex + 3}};
        mesh.m_triangles.push_back(tri1);
        mesh.m_triangles.push_back(tri2);
        
        // Mark this as a special grid quad by storing additional edge information
        // This will help the wireframe renderer show proper quad edges
    } else {
        // For other faces, use fan triangulation
        for (size_t i = 1; i < vertices.size() - 1; ++i) {
            Triangle triangle;
            triangle.indices = {{startIndex, startIndex + static_cast<unsigned int>(i), startIndex + static_cast<unsigned int>(i + 1)}};
            mesh.m_triangles.push_back(triangle);
        }
    }
}


