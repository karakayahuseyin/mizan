#include "Tesselator.h"
#include "../modeller/Face.h"
#include "../modeller/Loop.h"
#include "../modeller/Vertex.h"
#include <cmath>
#include <set>

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
    
    // Calculate face normal using first three vertices
    std::array<float, 3> normal = {0.0f, 0.0f, 0.0f};
    if (vertices.size() >= 3) {
        auto v1 = vertices[1]->getPosition() - vertices[0]->getPosition();
        auto v2 = vertices[2]->getPosition() - vertices[0]->getPosition();
        
        normal[0] = v1.y * v2.z - v1.z * v2.y;
        normal[1] = v1.z * v2.x - v1.x * v2.z;
        normal[2] = v1.x * v2.y - v1.y * v2.x;
        
        // Normalize
        float length = std::sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        if (length > 0.0f) {
            normal[0] /= length;
            normal[1] /= length;
            normal[2] /= length;
        }
    }
    
    // Store starting index for this face
    unsigned int startIndex = static_cast<unsigned int>(mesh.m_vertices.size());
    
    // Add vertices to mesh
    for (const auto& vertex : vertices) {
        RenderVertex renderVertex;
        auto pos = vertex->getPosition();
        renderVertex.position = {{static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z)}};
        renderVertex.normal = normal;
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

// Static helper functions for creating 3D meshes
Mesh MeshGenerator::createCube(float size) {
    Mesh mesh;
    float half = size * 0.5f;
    
    // Define cube vertices
    std::vector<std::array<float, 3>> positions = {
        // Front face
        {-half, -half,  half}, { half, -half,  half}, { half,  half,  half}, {-half,  half,  half},
        // Back face
        {-half, -half, -half}, {-half,  half, -half}, { half,  half, -half}, { half, -half, -half},
        // Top face
        {-half,  half, -half}, {-half,  half,  half}, { half,  half,  half}, { half,  half, -half},
        // Bottom face
        {-half, -half, -half}, { half, -half, -half}, { half, -half,  half}, {-half, -half,  half},
        // Right face
        { half, -half, -half}, { half,  half, -half}, { half,  half,  half}, { half, -half,  half},
        // Left face
        {-half, -half, -half}, {-half, -half,  half}, {-half,  half,  half}, {-half,  half, -half}
    };
    
    // Define normals for each face
    std::vector<std::array<float, 3>> normals = {
        // Front, Back, Top, Bottom, Right, Left
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
        {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
        {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0},
        {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0},
        {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
        {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}
    };
    
    for (size_t i = 0; i < positions.size(); ++i) {
        RenderVertex vertex;
        vertex.position = positions[i];
        vertex.normal = normals[i];
        mesh.m_vertices.push_back(vertex);
    }
    
    // Define triangles (2 per face)
    std::vector<std::array<unsigned int, 3>> indices = {
        // Front face
        {0, 1, 2}, {2, 3, 0},
        // Back face
        {4, 5, 6}, {6, 7, 4},
        // Top face
        {8, 9, 10}, {10, 11, 8},
        // Bottom face
        {12, 13, 14}, {14, 15, 12},
        // Right face
        {16, 17, 18}, {18, 19, 16},
        // Left face
        {20, 21, 22}, {22, 23, 20}
    };
    
    for (const auto& tri : indices) {
        Triangle triangle;
        triangle.indices = tri;
        mesh.m_triangles.push_back(triangle);
    }
    
    return mesh;
}

Mesh MeshGenerator::createPyramid(float size) {
    Mesh mesh;
    float half = size * 0.5f;
    float height = size * 0.8f;
    
    // Define pyramid vertices
    RenderVertex vertices[5] = {
        // Base vertices
        {{-half, 0, -half}, {0, -1, 0}},
        {{ half, 0, -half}, {0, -1, 0}},
        {{ half, 0,  half}, {0, -1, 0}},
        {{-half, 0,  half}, {0, -1, 0}},
        // Apex
        {{0, height, 0}, {0, 1, 0}}
    };
    
    for (int i = 0; i < 5; ++i) {
        mesh.m_vertices.push_back(vertices[i]);
    }
    
    // Define triangles
    Triangle triangles[6] = {
        // Base (2 triangles)
        {{0, 2, 1}}, {{0, 3, 2}},
        // Sides
        {{0, 1, 4}}, {{1, 2, 4}}, {{2, 3, 4}}, {{3, 0, 4}}
    };
    
    for (int i = 0; i < 6; ++i) {
        mesh.m_triangles.push_back(triangles[i]);
    }
    
    return mesh;
}

Mesh MeshGenerator::createGrid(int size, float spacing) {
    Mesh mesh;
    float halfSize = size * spacing * 0.5f;
    
    // Create grid lines
    for (int i = 0; i <= size; ++i) {
        float pos = -halfSize + i * spacing;
        
        // Horizontal lines
        mesh.m_vertices.push_back({{-halfSize, 0, pos}, {0, 1, 0}});
        mesh.m_vertices.push_back({{ halfSize, 0, pos}, {0, 1, 0}});
        
        // Vertical lines
        mesh.m_vertices.push_back({{pos, 0, -halfSize}, {0, 1, 0}});
        mesh.m_vertices.push_back({{pos, 0,  halfSize}, {0, 1, 0}});
    }
    
    return mesh;
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
