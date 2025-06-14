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
