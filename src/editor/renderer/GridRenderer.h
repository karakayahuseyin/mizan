#ifndef GRID_RENDERER_H
#define GRID_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Shader;

/**
 * GridRenderer handles direct OpenGL rendering of a grid
 * without using BREP topology. This provides better performance
 * and more control over grid appearance.
 */
class GridRenderer {
public:
    GridRenderer();
    ~GridRenderer();
    
    bool initialize();
    void cleanup();
    
    void render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
    
    // Grid properties
    void setGridSize(float size);
    void setGridSpacing(float spacing);
    void setGridColor(const glm::vec3& color);
    void regenerateGrid();
    
private:
    // OpenGL objects
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    std::unique_ptr<Shader> m_gridShader;
    
    // Grid properties
    float m_gridSize = 20.0f;
    float m_gridSpacing = 0.5f;
    glm::vec3 m_gridColor = glm::vec3(0.5f, 0.5f, 0.5f);
    
    // Vertex data
    std::vector<glm::vec3> m_vertices;
    bool m_needsUpdate = true;
    
    void generateGridVertices();
    void updateBuffers();
    bool createShader();
};

#endif // GRID_RENDERER_H
