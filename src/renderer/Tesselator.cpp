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
    
    // We'll create separate vertices for each face with their own normals
    // This avoids normal interpolation issues at shared edges
    
    // 1. Create the base (bottom face) - 4 vertices + 2 triangles
    unsigned int baseStart = 0;
    std::array<float, 3> baseNormal = {0.0f, -1.0f, 0.0f};
    
    // Base vertices (counter-clockwise when viewed from below)
    RenderVertex v0, v1, v2, v3;
    v0.position = {-half, 0.0f, -half};  // back-left
    v1.position = { half, 0.0f, -half};  // back-right
    v2.position = { half, 0.0f,  half};  // front-right
    v3.position = {-half, 0.0f,  half};  // front-left
    
    // All base vertices have same normal (pointing down)
    v0.normal = baseNormal;
    v1.normal = baseNormal;
    v2.normal = baseNormal;
    v3.normal = baseNormal;
    
    mesh.m_vertices.push_back(v0);
    mesh.m_vertices.push_back(v1);
    mesh.m_vertices.push_back(v2);
    mesh.m_vertices.push_back(v3);
    
    // Base triangles (counter-clockwise when viewed from below)
    mesh.m_triangles.push_back(Triangle(0, 1, 2));
    mesh.m_triangles.push_back(Triangle(0, 2, 3));
    
    // 2. Create each side face with its own vertices and correct normals
    // Each face has 3 vertices (2 from base + apex) and one triangle
    
    // Apex position
    std::array<float, 3> apexPos = {0.0f, height, 0.0f};
    
    // Front face
    unsigned int frontStart = mesh.m_vertices.size();
    {
        // Calculate normal for front face (using cross product)
        glm::vec3 edge1(v2.position[0] - v3.position[0], 
                        v2.position[1] - v3.position[1], 
                        v2.position[2] - v3.position[2]);
        glm::vec3 edge2(apexPos[0] - v3.position[0], 
                        apexPos[1] - v3.position[1], 
                        apexPos[2] - v3.position[2]);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        std::array<float, 3> frontNormal = {normal.x, normal.y, normal.z};
        
        // Create vertices with this normal
        RenderVertex fv0, fv1, fv2;
        fv0.position = v3.position;  // front-left
        fv0.normal = frontNormal;
        
        fv1.position = v2.position;  // front-right
        fv1.normal = frontNormal;
        
        fv2.position = apexPos;      // apex
        fv2.normal = frontNormal;
        
        mesh.m_vertices.push_back(fv0);
        mesh.m_vertices.push_back(fv1);
        mesh.m_vertices.push_back(fv2);
        
        // Add triangle (counter-clockwise)
        mesh.m_triangles.push_back(Triangle(frontStart, frontStart + 1, frontStart + 2));
    }
    
    // Right face
    unsigned int rightStart = mesh.m_vertices.size();
    {
        // Calculate normal for right face
        glm::vec3 edge1(v1.position[0] - v2.position[0], 
                        v1.position[1] - v2.position[1], 
                        v1.position[2] - v2.position[2]);
        glm::vec3 edge2(apexPos[0] - v2.position[0], 
                        apexPos[1] - v2.position[1], 
                        apexPos[2] - v2.position[2]);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        std::array<float, 3> rightNormal = {normal.x, normal.y, normal.z};
        
        // Create vertices with this normal
        RenderVertex rv0, rv1, rv2;
        rv0.position = v2.position;  // front-right
        rv0.normal = rightNormal;
        
        rv1.position = v1.position;  // back-right
        rv1.normal = rightNormal;
        
        rv2.position = apexPos;      // apex
        rv2.normal = rightNormal;
        
        mesh.m_vertices.push_back(rv0);
        mesh.m_vertices.push_back(rv1);
        mesh.m_vertices.push_back(rv2);
        
        // Add triangle (counter-clockwise)
        mesh.m_triangles.push_back(Triangle(rightStart, rightStart + 1, rightStart + 2));
    }
    
    // Back face
    unsigned int backStart = mesh.m_vertices.size();
    {
        // Calculate normal for back face
        glm::vec3 edge1(v0.position[0] - v1.position[0], 
                        v0.position[1] - v1.position[1], 
                        v0.position[2] - v1.position[2]);
        glm::vec3 edge2(apexPos[0] - v1.position[0], 
                        apexPos[1] - v1.position[1], 
                        apexPos[2] - v1.position[2]);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        std::array<float, 3> backNormal = {normal.x, normal.y, normal.z};
        
        // Create vertices with this normal
        RenderVertex bv0, bv1, bv2;
        bv0.position = v1.position;  // back-right
        bv0.normal = backNormal;
        
        bv1.position = v0.position;  // back-left
        bv1.normal = backNormal;
        
        bv2.position = apexPos;      // apex
        bv2.normal = backNormal;
        
        mesh.m_vertices.push_back(bv0);
        mesh.m_vertices.push_back(bv1);
        mesh.m_vertices.push_back(bv2);
        
        // Add triangle (counter-clockwise)
        mesh.m_triangles.push_back(Triangle(backStart, backStart + 1, backStart + 2));
    }
    
    // Left face
    unsigned int leftStart = mesh.m_vertices.size();
    {
        // Calculate normal for left face
        glm::vec3 edge1(v3.position[0] - v0.position[0], 
                        v3.position[1] - v0.position[1], 
                        v3.position[2] - v0.position[2]);
        glm::vec3 edge2(apexPos[0] - v0.position[0], 
                        apexPos[1] - v0.position[1], 
                        apexPos[2] - v0.position[2]);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        std::array<float, 3> leftNormal = {normal.x, normal.y, normal.z};
        
        // Create vertices with this normal
        RenderVertex lv0, lv1, lv2;
        lv0.position = v0.position;  // back-left
        lv0.normal = leftNormal;
        
        lv1.position = v3.position;  // front-left
        lv1.normal = leftNormal;
        
        lv2.position = apexPos;      // apex
        lv2.normal = leftNormal;
        
        mesh.m_vertices.push_back(lv0);
        mesh.m_vertices.push_back(lv1);
        mesh.m_vertices.push_back(lv2);
        
        // Add triangle (counter-clockwise)
        mesh.m_triangles.push_back(Triangle(leftStart, leftStart + 1, leftStart + 2));
    }
    
    return mesh;
}

Mesh MeshGenerator::createGrid(int size, float spacing) {
    Mesh mesh;
    float halfSize = size * spacing * 0.5f;
    
    // Create grid as a flat mesh with triangles
    std::array<float, 3> normal = {0, 1, 0};
    
    // Generate vertices in a grid pattern
    for (int i = 0; i <= size; ++i) {
        for (int j = 0; j <= size; ++j) {
            float x = -halfSize + i * spacing;
            float z = -halfSize + j * spacing;
            
            RenderVertex vertex;
            vertex.position = {x, 0.0f, z};
            vertex.normal = normal;
            mesh.m_vertices.push_back(vertex);
        }
    }
    
    // Generate triangles
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            // Calculate vertex indices for the quad
            unsigned int topLeft = i * (size + 1) + j;
            unsigned int topRight = i * (size + 1) + (j + 1);
            unsigned int bottomLeft = (i + 1) * (size + 1) + j;
            unsigned int bottomRight = (i + 1) * (size + 1) + (j + 1);
            
            // Create two triangles for each grid cell
            mesh.m_triangles.push_back(Triangle(topLeft, bottomLeft, topRight));
            mesh.m_triangles.push_back(Triangle(topRight, bottomLeft, bottomRight));
        }
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
