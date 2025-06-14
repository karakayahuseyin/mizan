#include "Renderer.h"
#include "Tesselator.h"
#include <iostream>
#include <GL/glu.h>

Renderer::Renderer() 
    : m_vertexArray(nullptr), m_gridEnabled(true) {
    // Initialize matrices
    for (int i = 0; i < 16; ++i) {
        m_viewMatrix[i] = 0.0f;
        m_projMatrix[i] = 0.0f;
    }
    // Set identity
    m_viewMatrix[0] = m_viewMatrix[5] = m_viewMatrix[10] = m_viewMatrix[15] = 1.0f;
    m_projMatrix[0] = m_projMatrix[5] = m_projMatrix[10] = m_projMatrix[15] = 1.0f;
}

Renderer::~Renderer() {
    cleanup();
}

void Renderer::initialize() {
    glewInit();
    
    m_vertexArray = std::make_unique<VertexArray>();
    
    // Create grid mesh
    m_gridMesh = MeshGenerator::createGrid(20, 0.5f);
    m_gridMesh.setColor(0.3f, 0.3f, 0.3f);
    
    // Set up basic lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Set up light properties
    GLfloat lightPos[] = {5.0f, 5.0f, 5.0f, 1.0f};
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    // Enable color material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void Renderer::loadMesh(const Mesh& mesh) {
    m_meshes.push_back(mesh);
    
    GLuint vbo, ebo, wireframeVbo, wireframeEbo;
    setupMeshBuffers(mesh, vbo, ebo, wireframeVbo, wireframeEbo);
    m_vbos.push_back(vbo);
    m_ebos.push_back(ebo);
    m_wireframeVbos.push_back(wireframeVbo);
    m_wireframeEbos.push_back(wireframeEbo);
}

void Renderer::render() {
    if (!m_vertexArray) return;
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render grid
    if (m_gridEnabled) {
        glPushMatrix();
        glColor3f(m_gridMesh.m_color[0], m_gridMesh.m_color[1], m_gridMesh.m_color[2]);
        glBegin(GL_LINES);
        for (size_t i = 0; i < m_gridMesh.m_vertices.size(); i += 2) {
            const auto& v1 = m_gridMesh.m_vertices[i];
            const auto& v2 = m_gridMesh.m_vertices[i + 1];
            glVertex3f(v1.position[0], v1.position[1], v1.position[2]);
            glVertex3f(v2.position[0], v2.position[1], v2.position[2]);
        }
        glEnd();
        glPopMatrix();
    }
    
    // Render meshes
    for (size_t i = 0; i < m_meshes.size(); ++i) {
        if (i < m_vbos.size() && i < m_ebos.size() && i < m_wireframeVbos.size() && i < m_wireframeEbos.size()) {
            renderMesh(m_meshes[i], m_vbos[i], m_ebos[i], m_wireframeVbos[i], m_wireframeEbos[i]);
        }
    }
}

void Renderer::renderMesh(const Mesh& mesh, GLuint vbo, GLuint ebo, GLuint wireframeVbo, GLuint wireframeEbo) {
    glPushMatrix();
    
    // Apply transformations
    glTranslatef(mesh.m_position[0], mesh.m_position[1], mesh.m_position[2]);
    glRotatef(mesh.m_rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(mesh.m_rotation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(mesh.m_rotation[2], 0.0f, 0.0f, 1.0f);
    glScalef(mesh.m_scale[0], mesh.m_scale[1], mesh.m_scale[2]);
    
    // Render solid mesh
    if (mesh.m_showSolid) {
        renderSolid(mesh, vbo, ebo);
    }
    
    // Render wireframe
    if (mesh.m_showWireframe) {
        renderWireframe(mesh, wireframeVbo, wireframeEbo);
    }
    
    glPopMatrix();
}

void Renderer::renderSolid(const Mesh& mesh, GLuint vbo, GLuint ebo) {
    // Ensure lighting is enabled for solid rendering
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Set material color
    GLfloat materialColor[] = {mesh.m_color[0], mesh.m_color[1], mesh.m_color[2], 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, materialColor);
    
    // Also set glColor for color material
    glColor3f(mesh.m_color[0], mesh.m_color[1], mesh.m_color[2]);
    
    // Bind buffers and render solid
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, sizeof(RenderVertex), (void*)offsetof(RenderVertex, position));
    glNormalPointer(GL_FLOAT, sizeof(RenderVertex), (void*)offsetof(RenderVertex, normal));
    
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.getTriangleCount() * 3), GL_UNSIGNED_INT, 0);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void Renderer::renderWireframe(const Mesh& mesh, GLuint wireframeVbo, GLuint wireframeEbo) {
    // Set wireframe color (yellow if selected, normal color otherwise)
    auto wireframeColor = mesh.getCurrentWireframeColor();
    glColor3f(wireframeColor[0], wireframeColor[1], wireframeColor[2]);
    
    // Disable lighting for wireframe
    glDisable(GL_LIGHTING);
    
    // Enable line smoothing for better appearance
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);
    
    // Enable polygon offset to prevent z-fighting
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    
    // Bind wireframe buffers and render
    glBindBuffer(GL_ARRAY_BUFFER, wireframeVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeEbo);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(RenderVertex), (void*)offsetof(RenderVertex, position));
    
    // Get the actual number of edge indices from the buffer
    GLint bufferSize;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    GLsizei indexCount = bufferSize / sizeof(unsigned int);
    
    glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glDisable(GL_POLYGON_OFFSET_LINE);
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_LIGHTING);
}

void Renderer::setupMeshBuffers(const Mesh& mesh, GLuint& vbo, GLuint& ebo, GLuint& wireframeVbo, GLuint& wireframeEbo) {
    // Setup solid mesh buffers
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.getVertexCount() * sizeof(RenderVertex), mesh.m_vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getTriangleCount() * sizeof(Triangle), mesh.m_triangles.data(), GL_STATIC_DRAW);
    
    // Setup wireframe buffers
    glGenBuffers(1, &wireframeVbo);
    glGenBuffers(1, &wireframeEbo);
    
    // Reuse vertex data for wireframe
    glBindBuffer(GL_ARRAY_BUFFER, wireframeVbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.getVertexCount() * sizeof(RenderVertex), mesh.m_vertices.data(), GL_STATIC_DRAW);
    
    // Create edge index buffer
    auto edges = mesh.getEdges();
    std::vector<unsigned int> edgeIndices;
    edgeIndices.reserve(edges.size() * 2);
    
    for (const auto& edge : edges) {
        edgeIndices.push_back(edge.first);
        edgeIndices.push_back(edge.second);
    }
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeEbo);
    if (!edgeIndices.empty()) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIndices.size() * sizeof(unsigned int), edgeIndices.data(), GL_STATIC_DRAW);
    } else {
        // Fallback: create wireframe from triangles
        std::vector<unsigned int> triangleEdges;
        for (const auto& triangle : mesh.m_triangles) {
            // Add each edge of the triangle
            triangleEdges.push_back(triangle.indices[0]);
            triangleEdges.push_back(triangle.indices[1]);
            
            triangleEdges.push_back(triangle.indices[1]);
            triangleEdges.push_back(triangle.indices[2]);
            
            triangleEdges.push_back(triangle.indices[2]);
            triangleEdges.push_back(triangle.indices[0]);
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleEdges.size() * sizeof(unsigned int), triangleEdges.data(), GL_STATIC_DRAW);
    }
}

void Renderer::cleanup() {
    for (GLuint vbo : m_vbos) {
        if (vbo != 0) glDeleteBuffers(1, &vbo);
    }
    for (GLuint ebo : m_ebos) {
        if (ebo != 0) glDeleteBuffers(1, &ebo);
    }
    for (GLuint wireframeVbo : m_wireframeVbos) {
        if (wireframeVbo != 0) glDeleteBuffers(1, &wireframeVbo);
    }
    for (GLuint wireframeEbo : m_wireframeEbos) {
        if (wireframeEbo != 0) glDeleteBuffers(1, &wireframeEbo);
    }
    
    m_vbos.clear();
    m_ebos.clear();
    m_wireframeVbos.clear();
    m_wireframeEbos.clear();
    m_vertexArray.reset();
    m_meshes.clear();
}

void Renderer::setViewMatrix(const std::array<float, 16>& viewMatrix) {
    m_viewMatrix = viewMatrix;
}

void Renderer::setProjectionMatrix(const std::array<float, 16>& projMatrix) {
    m_projMatrix = projMatrix;
}

void Renderer::updateMesh(size_t index, const Mesh& mesh) {
    if (index < m_meshes.size()) {
        m_meshes[index] = mesh;
    }
}

void Renderer::updateAllMeshes(const std::vector<Mesh>& meshes) {
    // Update existing meshes without reallocating buffers
    size_t minSize = std::min(m_meshes.size(), meshes.size());
    for (size_t i = 0; i < minSize; ++i) {
        m_meshes[i] = meshes[i];
    }
}

