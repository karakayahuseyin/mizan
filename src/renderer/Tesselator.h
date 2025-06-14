#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include "../modeller/Solid.h"
#include <vector>
#include <array>

struct RenderVertex {
    std::array<float, 3> position;
    std::array<float, 3> normal;
};

struct Triangle {
    std::array<unsigned int, 3> indices;
};

class Mesh {
public:
    std::vector<RenderVertex> m_vertices;
    std::vector<Triangle> m_triangles;
    
    // Mesh properties
    std::array<float, 3> m_color = {1.0f, 1.0f, 1.0f};
    std::array<float, 3> m_rotation = {0.0f, 0.0f, 0.0f}; // X, Y, Z rotations
    std::array<float, 3> m_position = {0.0f, 0.0f, 0.0f};
    std::array<float, 3> m_scale = {1.0f, 1.0f, 1.0f};
    
    // Rendering properties
    bool m_showWireframe = true;
    bool m_showSolid = true;
    std::array<float, 3> m_wireframeColor = {1.0f, 1.0f, 1.0f}; // White edges
    
    void clear();
    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getTriangleCount() const { return m_triangles.size(); }
    
    void setColor(float r, float g, float b);
    void setRotation(float x, float y, float z);
    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setWireframeColor(float r, float g, float b);
    
    // Edge extraction for wireframe
    std::vector<std::pair<unsigned int, unsigned int>> getEdges() const;
};

class Tessellator {
public:
    static Mesh tessellate(const BREP::Solid& solid);
    
private:
    static void processShell(const std::shared_ptr<BREP::Shell>& shell, Mesh& mesh);
    static void processFace(const std::shared_ptr<BREP::Face>& face, Mesh& mesh);
};

// Mesh generation utilities
class MeshGenerator {
public:
    static Mesh createCube(float size = 1.0f);
    static Mesh createPyramid(float size = 1.0f);
    static Mesh createGrid(int size = 10, float spacing = 1.0f);
};

#endif