#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "Tesselator.h"
#include "VertexArray.h"
#include <memory>
#include <array>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initialize();
    void loadMesh(const Mesh& mesh);
    void render();
    void cleanup();
    
    void setViewMatrix(const std::array<float, 16>& viewMatrix);
    void setProjectionMatrix(const std::array<float, 16>& projMatrix);
    void enableGrid(bool enable) { m_gridEnabled = enable; }
    
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
    
    // Grid
    Mesh m_gridMesh;
    bool m_gridEnabled = true;
    
    // Matrices
    std::array<float, 16> m_viewMatrix;
    std::array<float, 16> m_projMatrix;
    
    void renderMesh(const Mesh& mesh, GLuint vbo, GLuint ebo, GLuint wireframeVbo, GLuint wireframeEbo);
    void setupMeshBuffers(const Mesh& mesh, GLuint& vbo, GLuint& ebo, GLuint& wireframeVbo, GLuint& wireframeEbo);
    void renderSolid(const Mesh& mesh, GLuint vbo, GLuint ebo);
    void renderWireframe(const Mesh& mesh, GLuint wireframeVbo, GLuint wireframeEbo);
};

#endif