#include "BREPBuilder.h"
#include "Shell.h"
#include "Face.h"
#include "Loop.h"
#include "Vertex.h"
#include "HalfEdge.h"
#include "Edge.h"
#include <memory>

namespace BREP {

Solid BREPBuilder::createCubeSolid(float size) {
    Solid solid;
    float half = size * 0.5f;
    
    // Create vertices
    std::vector<VertexPtr> vertices;
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(-half, -half, -half))); // 0
    vertices.push_back(std::make_shared<Vertex>(glm::vec3( half, -half, -half))); // 1
    vertices.push_back(std::make_shared<Vertex>(glm::vec3( half,  half, -half))); // 2
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(-half,  half, -half))); // 3
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(-half, -half,  half))); // 4
    vertices.push_back(std::make_shared<Vertex>(glm::vec3( half, -half,  half))); // 5
    vertices.push_back(std::make_shared<Vertex>(glm::vec3( half,  half,  half))); // 6
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(-half,  half,  half))); // 7
    
    // Create shell
    auto shell = std::make_shared<Shell>();
    
    // Create faces with simple loops
    // Front face (z = half)
    std::vector<VertexPtr> frontVertices = {vertices[4], vertices[5], vertices[6], vertices[7]};
    auto frontLoop = createSimpleLoop(frontVertices);
    auto frontFace = std::make_shared<Face>(frontLoop);
    shell->addFace(frontFace);
    
    // Back face (z = -half)
    std::vector<VertexPtr> backVertices = {vertices[1], vertices[0], vertices[3], vertices[2]};
    auto backLoop = createSimpleLoop(backVertices);
    auto backFace = std::make_shared<Face>(backLoop);
    shell->addFace(backFace);
    
    // Bottom face (y = -half)
    std::vector<VertexPtr> bottomVertices = {vertices[0], vertices[1], vertices[5], vertices[4]};
    auto bottomLoop = createSimpleLoop(bottomVertices);
    auto bottomFace = std::make_shared<Face>(bottomLoop);
    shell->addFace(bottomFace);
    
    // Top face (y = half)
    std::vector<VertexPtr> topVertices = {vertices[3], vertices[7], vertices[6], vertices[2]};
    auto topLoop = createSimpleLoop(topVertices);
    auto topFace = std::make_shared<Face>(topLoop);
    shell->addFace(topFace);
    
    // Left face (x = -half)
    std::vector<VertexPtr> leftVertices = {vertices[0], vertices[4], vertices[7], vertices[3]};
    auto leftLoop = createSimpleLoop(leftVertices);
    auto leftFace = std::make_shared<Face>(leftLoop);
    shell->addFace(leftFace);
    
    // Right face (x = half)
    std::vector<VertexPtr> rightVertices = {vertices[1], vertices[2], vertices[6], vertices[5]};
    auto rightLoop = createSimpleLoop(rightVertices);
    auto rightFace = std::make_shared<Face>(rightLoop);
    shell->addFace(rightFace);
    
    solid.addShell(shell);
    return solid;
}

Solid BREPBuilder::createPyramidSolid(float size) {
    Solid solid;
    float half = size * 0.5f;
    float height = size * 0.8f;
    
    // Create vertices
    std::vector<VertexPtr> vertices;
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(-half, 0.0f, -half))); // 0 - base back-left
    vertices.push_back(std::make_shared<Vertex>(glm::vec3( half, 0.0f, -half))); // 1 - base back-right
    vertices.push_back(std::make_shared<Vertex>(glm::vec3( half, 0.0f,  half))); // 2 - base front-right
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(-half, 0.0f,  half))); // 3 - base front-left
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(0.0f, height, 0.0f))); // 4 - apex
    
    // Create shell
    auto shell = std::make_shared<Shell>();
    
    // Base face
    std::vector<VertexPtr> baseVertices = {vertices[0], vertices[1], vertices[2], vertices[3]};
    auto baseLoop = createSimpleLoop(baseVertices);
    auto baseFace = std::make_shared<Face>(baseLoop);
    shell->addFace(baseFace);
    
    // Front face
    std::vector<VertexPtr> frontVertices = {vertices[3], vertices[2], vertices[4]};
    auto frontLoop = createSimpleLoop(frontVertices);
    auto frontFace = std::make_shared<Face>(frontLoop);
    shell->addFace(frontFace);
    
    // Right face
    std::vector<VertexPtr> rightVertices = {vertices[2], vertices[1], vertices[4]};
    auto rightLoop = createSimpleLoop(rightVertices);
    auto rightFace = std::make_shared<Face>(rightLoop);
    shell->addFace(rightFace);
    
    // Back face
    std::vector<VertexPtr> backVertices = {vertices[1], vertices[0], vertices[4]};
    auto backLoop = createSimpleLoop(backVertices);
    auto backFace = std::make_shared<Face>(backLoop);
    shell->addFace(backFace);
    
    // Left face
    std::vector<VertexPtr> leftVertices = {vertices[0], vertices[3], vertices[4]};
    auto leftLoop = createSimpleLoop(leftVertices);
    auto leftFace = std::make_shared<Face>(leftLoop);
    shell->addFace(leftFace);
    
    solid.addShell(shell);
    return solid;
}

Solid BREPBuilder::createGridSolid(int size, float spacing) {
    Solid solid;
    float halfSize = size * spacing * 0.5f;
    
    // Create vertices in a grid pattern
    std::vector<std::vector<VertexPtr>> gridVertices(size + 1, std::vector<VertexPtr>(size + 1));
    
    for (int i = 0; i <= size; ++i) {
        for (int j = 0; j <= size; ++j) {
            float x = -halfSize + i * spacing;
            float z = -halfSize + j * spacing;
            gridVertices[i][j] = std::make_shared<Vertex>(glm::vec3(x, 0.0f, z));
        }
    }
    
    // Create shell
    auto shell = std::make_shared<Shell>();
    
    // Create faces for each grid cell
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            // Create two triangular faces for each grid cell
            
            // First triangle
            std::vector<VertexPtr> tri1Vertices = {
                gridVertices[i][j],
                gridVertices[i+1][j],
                gridVertices[i][j+1]
            };
            auto loop1 = createSimpleLoop(tri1Vertices);
            auto face1 = std::make_shared<Face>(loop1);
            shell->addFace(face1);
            
            // Second triangle
            std::vector<VertexPtr> tri2Vertices = {
                gridVertices[i][j+1],
                gridVertices[i+1][j],
                gridVertices[i+1][j+1]
            };
            auto loop2 = createSimpleLoop(tri2Vertices);
            auto face2 = std::make_shared<Face>(loop2);
            shell->addFace(face2);
        }
    }
    
    solid.addShell(shell);
    return solid;
}

Solid BREPBuilder::createCylinderSolid(float radius, float height, int segments) {
    Solid solid;
    float halfHeight = height * 0.5f;
    
    // Create vertices
    std::vector<VertexPtr> vertices;
    
    // Bottom center
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(0.0f, -halfHeight, 0.0f)));
    
    // Bottom ring
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(std::make_shared<Vertex>(glm::vec3(x, -halfHeight, z)));
    }
    
    // Top ring
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(std::make_shared<Vertex>(glm::vec3(x, halfHeight, z)));
    }
    
    // Top center
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(0.0f, halfHeight, 0.0f)));
    
    // Create shell
    auto shell = std::make_shared<Shell>();
    
    // Create bottom cap (counter-clockwise when viewed from below)
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        std::vector<VertexPtr> triVertices = {
            vertices[0],                // bottom center
            vertices[1 + i],            // current bottom vertex
            vertices[1 + next]          // next bottom vertex
        };
        auto loop = createSimpleLoop(triVertices);
        auto face = std::make_shared<Face>(loop);
        shell->addFace(face);
    }
    
    // Create side faces (counter-clockwise when viewed from outside)
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        int bottomCurrent = 1 + i;
        int bottomNext = 1 + next;
        int topCurrent = 1 + segments + i;
        int topNext = 1 + segments + next;
        
        // First triangle (bottom-left, top-left, bottom-right)
        std::vector<VertexPtr> tri1Vertices = {
            vertices[bottomCurrent],
            vertices[topCurrent],
            vertices[bottomNext]
        };
        auto loop1 = createSimpleLoop(tri1Vertices);
        auto face1 = std::make_shared<Face>(loop1);
        shell->addFace(face1);
        
        // Second triangle (bottom-right, top-left, top-right)
        std::vector<VertexPtr> tri2Vertices = {
            vertices[bottomNext],
            vertices[topCurrent],
            vertices[topNext]
        };
        auto loop2 = createSimpleLoop(tri2Vertices);
        auto face2 = std::make_shared<Face>(loop2);
        shell->addFace(face2);
    }
    
    // Create top cap (counter-clockwise when viewed from above)
    int topCenterIndex = vertices.size() - 1;
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        std::vector<VertexPtr> triVertices = {
            vertices[topCenterIndex],       // top center
            vertices[1 + segments + next],  // next top vertex
            vertices[1 + segments + i]      // current top vertex
        };
        auto loop = createSimpleLoop(triVertices);
        auto face = std::make_shared<Face>(loop);
        shell->addFace(face);
    }
    
    solid.addShell(shell);
    return solid;
}

Solid BREPBuilder::createSphereSolid(float radius, int latitudeSegments, int longitudeSegments) {
    Solid solid;
    
    // Create vertices
    std::vector<VertexPtr> vertices;
    
    // Add top pole
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(0.0f, radius, 0.0f)));
    
    // Add latitude rings
    for (int lat = 1; lat < latitudeSegments; ++lat) {
        float theta = M_PI * lat / latitudeSegments;
        float y = radius * cos(theta);
        float ringRadius = radius * sin(theta);
        
        for (int lon = 0; lon < longitudeSegments; ++lon) {
            float phi = 2.0f * M_PI * lon / longitudeSegments;
            float x = ringRadius * cos(phi);
            float z = ringRadius * sin(phi);
            vertices.push_back(std::make_shared<Vertex>(glm::vec3(x, y, z)));
        }
    }
    
    // Add bottom pole
    vertices.push_back(std::make_shared<Vertex>(glm::vec3(0.0f, -radius, 0.0f)));
    
    // Create shell
    auto shell = std::make_shared<Shell>();
    
    // Create top cap triangles (counter-clockwise when viewed from outside)
    for (int lon = 0; lon < longitudeSegments; ++lon) {
        int next = (lon + 1) % longitudeSegments;
        std::vector<VertexPtr> triVertices = {
            vertices[0],                    // top pole
            vertices[1 + next],             // next longitude
            vertices[1 + lon]               // current longitude
        };
        auto loop = createSimpleLoop(triVertices);
        auto face = std::make_shared<Face>(loop);
        shell->addFace(face);
    }
    
    // Create middle quads (as triangles) - counter-clockwise
    for (int lat = 0; lat < latitudeSegments - 2; ++lat) {
        for (int lon = 0; lon < longitudeSegments; ++lon) {
            int next = (lon + 1) % longitudeSegments;
            int currentRing = 1 + lat * longitudeSegments;
            int nextRing = 1 + (lat + 1) * longitudeSegments;
            
            // First triangle
            std::vector<VertexPtr> tri1Vertices = {
                vertices[currentRing + lon],
                vertices[currentRing + next],
                vertices[nextRing + lon]
            };
            auto loop1 = createSimpleLoop(tri1Vertices);
            auto face1 = std::make_shared<Face>(loop1);
            shell->addFace(face1);
            
            // Second triangle
            std::vector<VertexPtr> tri2Vertices = {
                vertices[currentRing + next],
                vertices[nextRing + next],
                vertices[nextRing + lon]
            };
            auto loop2 = createSimpleLoop(tri2Vertices);
            auto face2 = std::make_shared<Face>(loop2);
            shell->addFace(face2);
        }
    }
    
    // Create bottom cap triangles (counter-clockwise when viewed from outside)
    int bottomPoleIndex = vertices.size() - 1;
    int lastRingStart = 1 + (latitudeSegments - 2) * longitudeSegments;
    for (int lon = 0; lon < longitudeSegments; ++lon) {
        int next = (lon + 1) % longitudeSegments;
        std::vector<VertexPtr> triVertices = {
            vertices[bottomPoleIndex],      // bottom pole
            vertices[lastRingStart + lon],  // current longitude
            vertices[lastRingStart + next]  // next longitude
        };
        auto loop = createSimpleLoop(triVertices);
        auto face = std::make_shared<Face>(loop);
        shell->addFace(face);
    }
    
    solid.addShell(shell);
    return solid;
}

LoopPtr BREPBuilder::createSimpleLoop(const std::vector<VertexPtr>& vertices) {
    if (vertices.size() < 3) {
        return nullptr;
    }
    
    // Create half-edges for each edge in the loop
    std::vector<HalfEdgePtr> halfEdges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto halfEdge = std::make_shared<HalfEdge>(vertices[i]);
        halfEdges.push_back(halfEdge);
    }
    
    // Connect the half-edges in a loop
    connectHalfEdges(halfEdges);
    
    // Create the loop with the first half-edge
    auto loop = std::make_shared<Loop>(halfEdges[0]);
    
    // Set the loop reference for all half-edges
    for (auto& he : halfEdges) {
        // Note: This is a simplified approach - in a full BREP implementation,
        // half-edges would have a loop reference, but our current Loop class
        // doesn't have this. For now, we'll just create the basic structure.
    }
    
    return loop;
}

void BREPBuilder::connectHalfEdges(const std::vector<HalfEdgePtr>& halfEdges) {
    if (halfEdges.empty()) return;
    
    // Connect each half-edge to the next one
    for (size_t i = 0; i < halfEdges.size(); ++i) {
        size_t nextIndex = (i + 1) % halfEdges.size();
        size_t prevIndex = (i + halfEdges.size() - 1) % halfEdges.size();
        
        halfEdges[i]->setNext(halfEdges[nextIndex]);
        halfEdges[i]->setPrev(halfEdges[prevIndex]);
        
        // Set up the reciprocal relationships manually to avoid shared_ptr issues
        if (halfEdges[nextIndex]) {
            halfEdges[nextIndex]->m_prev = halfEdges[i];
        }
        if (halfEdges[prevIndex]) {
            halfEdges[prevIndex]->m_next = halfEdges[i];
        }
    }
}

} // namespace BREP
