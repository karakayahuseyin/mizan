#include "Mesh.h"

#include <algorithm>
#include <set>


void Mesh::clear() {
    m_vertices.clear();
    m_triangles.clear();
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
    
    // For grid meshes, we want to show quad edges, not triangle edges
    // Check if this looks like a grid (many quads in a regular pattern)
    bool isGrid = (m_triangles.size() % 2 == 0) && (m_vertices.size() > 16);
    
    if (isGrid) {
        // Create quad-based edges for grid
        for (size_t i = 0; i < m_triangles.size(); i += 2) {
            if (i + 1 < m_triangles.size()) {
                const auto& tri1 = m_triangles[i];
                const auto& tri2 = m_triangles[i + 1];
                
                // Find the shared edge between the two triangles
                std::set<unsigned int> tri1Verts = {tri1.indices[0], tri1.indices[1], tri1.indices[2]};
                std::set<unsigned int> tri2Verts = {tri2.indices[0], tri2.indices[1], tri2.indices[2]};
                
                // Find unique vertices (quad corners)
                std::set<unsigned int> quadVerts;
                for (auto v : tri1Verts) quadVerts.insert(v);
                for (auto v : tri2Verts) quadVerts.insert(v);
                
                if (quadVerts.size() == 4) {
                    // Convert to vector for ordering
                    std::vector<unsigned int> verts(quadVerts.begin(), quadVerts.end());
                    
                    // Add quad edges (assuming counter-clockwise ordering)
                    for (size_t j = 0; j < 4; ++j) {
                        unsigned int v1 = verts[j];
                        unsigned int v2 = verts[(j + 1) % 4];
                        if (v1 > v2) std::swap(v1, v2);
                        edgeSet.insert({v1, v2});
                    }
                }
            }
        }
    } else {
        // Regular triangle-based edge extraction
        for (const auto& triangle : m_triangles) {
            for (int i = 0; i < 3; ++i) {
                unsigned int v1 = triangle.indices[i];
                unsigned int v2 = triangle.indices[(i + 1) % 3];
                
                // Ensure consistent edge ordering (smaller index first)
                if (v1 > v2) std::swap(v1, v2);
                edgeSet.insert({v1, v2});
            }
        }
    }
    
    // Convert set to vector
    edges.reserve(edgeSet.size());
    for (const auto& edge : edgeSet) {
        edges.push_back(edge);
    }
    
    return edges;
}