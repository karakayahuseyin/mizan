#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "Tessellator.h"
#include "VertexArray.h"
#include "Shader.h"
#include "GridRenderer.h"
#include <memory>
#include <array>
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initialize();
    void loadMesh(const Mesh& mesh);
    void render();
    void cleanup();
    
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setProjectionMatrix(const glm::mat4& projMatrix);
    void setCameraPosition(const glm::vec3& position);
    void setBackgroundColor(float r, float g, float b) { m_backgroundColor = glm::vec3(r, g, b); }
    glm::vec3 getBackgroundColor() const { return m_backgroundColor; }
    
    // Grid control
    void updateGridSettings(); // Updates grid from Settings singleton
    
    // Viewport resize handling
    void resize(int width, int height);
    
    // Update existing mesh properties
    void updateMesh(size_t index, const Mesh& mesh);
    void updateAllMeshes(const std::vector<Mesh>& meshes);

private:
    std::unique_ptr<VertexArray> m_vertexArray;
    std::vector<GLuint> m_vbos;
    std::vector<GLuint> m_ebos;
    std::vector<GLuint> m_wireframeVbos; // Vertex buffers for wireframe
    std::vector<GLuint> m_wireframeEbos; // Element buffers for wireframe
    std::vector<Mesh> m_meshes;
    
    // Shaders
    std::unique_ptr<Shader> m_basicShader;
    std::unique_ptr<Shader> m_wireframeShader;
    
    // Grid rendering
    std::unique_ptr<GridRenderer> m_gridRenderer;
    glm::vec3 m_backgroundColor = glm::vec3(0.1f, 0.1f, 0.1f); // Default dark background
    int m_viewportWidth = 800;
    int m_viewportHeight = 600;
    
    // Matrices and lighting
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;
    glm::vec3 m_cameraPos;
    glm::vec3 m_lightPos;
    glm::vec3 m_lightColor;
    
    void renderMesh(const Mesh& mesh, GLuint vbo, GLuint ebo, GLuint wireframeVbo, GLuint wireframeEbo);
    void setupMeshBuffers(const Mesh& mesh, GLuint& vbo, GLuint& ebo, GLuint& wireframeVbo, GLuint& wireframeEbo);
    void renderSolid(const Mesh& mesh, GLuint vbo, GLuint ebo);
    void renderWireframe(const Mesh& mesh, GLuint wireframeVbo, GLuint wireframeEbo);
    
    bool initializeShaders();
    void setupVertexAttributes();
};

#endif