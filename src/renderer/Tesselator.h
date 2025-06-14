#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include "../modeller/Solid.h"
#include "../modeller/BREPBuilder.h"
#include <vector>
#include <array>

struct RenderVertex {
    std::array<float, 3> position;
    std::array<float, 3> normal;
};

struct Triangle {
    std::array<unsigned int, 3> indices;
    
    Triangle() = default;
    Triangle(unsigned int i0, unsigned int i1, unsigned int i2) : indices{{i0, i1, i2}} {}
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
    
    // Selection state
    bool m_isSelected = false;
    std::array<float, 3> m_selectedWireframeColor = {1.0f, 1.0f, 0.0f}; // Yellow for selection
    
    void clear();
    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getTriangleCount() const { return m_triangles.size(); }
    
    void setColor(float r, float g, float b);
    void setRotation(float x, float y, float z);
    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setWireframeColor(float r, float g, float b);
    void setSelected(bool selected);
    bool isSelected() const { return m_isSelected; }
    std::array<float, 3> getCurrentWireframeColor() const;
    
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

#endif