#include "Renderer.h"
#include "Tesselator.h"
#include <iostream>
#include <GL/glu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

Renderer::Renderer() 
    : m_vertexArray(nullptr), m_gridEnabled(true) {
    // Initialize matrices
    m_viewMatrix = glm::mat4(1.0f);
    m_projMatrix = glm::mat4(1.0f);
}

Renderer::~Renderer() {
    cleanup();
}

void Renderer::initialize() {
    glewInit();
    
    m_vertexArray = std::make_unique<VertexArray>();
    
    // Initialize shaders
    if (!initializeShaders()) {
        std::cerr << "Failed to initialize shaders!" << std::endl;
        return;
    }
    
    // Create grid mesh and setup its buffers
    m_gridMesh = MeshGenerator::createGrid(20, 0.5f);
    m_gridMesh.setColor(0.3f, 0.3f, 0.3f);
    m_gridMesh.setWireframeColor(0.3f, 0.3f, 0.3f);
    m_gridMesh.m_showSolid = false;  // Grid should only show wireframe
    m_gridMesh.m_showWireframe = true;
    
    // Setup grid buffers
    GLuint gridVbo, gridEbo, gridWireframeVbo, gridWireframeEbo;
    setupMeshBuffers(m_gridMesh, gridVbo, gridEbo, gridWireframeVbo, gridWireframeEbo);
    m_vbos.insert(m_vbos.begin(), gridVbo);
    m_ebos.insert(m_ebos.begin(), gridEbo);
    m_wireframeVbos.insert(m_wireframeVbos.begin(), gridWireframeVbo);
    m_wireframeEbos.insert(m_wireframeEbos.begin(), gridWireframeEbo);
    
    // Initialize lighting
    m_lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    m_cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    
    // Initialize matrices
    m_viewMatrix = glm::mat4(1.0f);
    m_projMatrix = glm::mat4(1.0f);
    
    // Configure OpenGL state for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Disable blending for solid surfaces
    glDisable(GL_BLEND);
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
    
    // Reset all OpenGL state to ensure proper rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Clear buffers
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render grid first (if enabled)
    if (m_gridEnabled && !m_vbos.empty()) {
        renderMesh(m_gridMesh, m_vbos[0], m_ebos[0], m_wireframeVbos[0], m_wireframeEbos[0]);
    }
    
    // Render regular meshes (skip grid at index 0)
    for (size_t i = 0; i < m_meshes.size(); ++i) {
        size_t bufferIndex = i + 1; // Offset by 1 to account for grid
        if (bufferIndex < m_vbos.size() && bufferIndex < m_ebos.size() && 
            bufferIndex < m_wireframeVbos.size() && bufferIndex < m_wireframeEbos.size()) {
            renderMesh(m_meshes[i], m_vbos[bufferIndex], m_ebos[bufferIndex], 
                      m_wireframeVbos[bufferIndex], m_wireframeEbos[bufferIndex]);
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
    // Force state for completely opaque solid rendering
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    // Clear any errors
    while (glGetError() != GL_NO_ERROR) {}
    
    m_basicShader->use();
    
    // Set uniforms
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mesh.m_position[0], mesh.m_position[1], mesh.m_position[2]));
    model = glm::rotate(model, glm::radians(mesh.m_rotation[0]), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(mesh.m_rotation[1]), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(mesh.m_rotation[2]), glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(mesh.m_scale[0], mesh.m_scale[1], mesh.m_scale[2]));
    
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    
    m_basicShader->setUniform("uModel", model);
    m_basicShader->setUniform("uView", m_viewMatrix);
    m_basicShader->setUniform("uProjection", m_projMatrix);
    m_basicShader->setUniform("uNormalMatrix", normalMatrix);
    m_basicShader->setUniform("uColor", glm::vec3(mesh.m_color[0], mesh.m_color[1], mesh.m_color[2]));
    m_basicShader->setUniform("uLightPos", m_lightPos);
    m_basicShader->setUniform("uLightColor", m_lightColor);
    m_basicShader->setUniform("uViewPos", m_cameraPos);
    
    // Bind VAO and render
    m_vertexArray->bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    setupVertexAttributes();
    
    // Draw with completely opaque settings
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.getTriangleCount() * 3), GL_UNSIGNED_INT, 0);
    
    // Check for any OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error in renderSolid: " << err << std::endl;
    }
    
    m_vertexArray->unbind();
    m_basicShader->unuse();
}

void Renderer::renderWireframe(const Mesh& mesh, GLuint wireframeVbo, GLuint wireframeEbo) {
    m_wireframeShader->use();
    
    // Set uniforms
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mesh.m_position[0], mesh.m_position[1], mesh.m_position[2]));
    model = glm::rotate(model, glm::radians(mesh.m_rotation[0]), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(mesh.m_rotation[1]), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(mesh.m_rotation[2]), glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(mesh.m_scale[0], mesh.m_scale[1], mesh.m_scale[2]));
    
    auto wireframeColor = mesh.getCurrentWireframeColor();
    
    m_wireframeShader->setUniform("uModel", model);
    m_wireframeShader->setUniform("uView", m_viewMatrix);
    m_wireframeShader->setUniform("uProjection", m_projMatrix);
    m_wireframeShader->setUniform("uColor", glm::vec3(wireframeColor[0], wireframeColor[1], wireframeColor[2]));
    
    // For wireframe rendering, disable face culling and enable line rendering
    glDisable(GL_CULL_FACE);
    glPolygonOffset(-1.0f, -1.0f);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);
    
    // Bind VAO and render
    m_vertexArray->bind();
    glBindBuffer(GL_ARRAY_BUFFER, wireframeVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeEbo);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    GLint bufferSize;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    GLsizei indexCount = bufferSize / sizeof(unsigned int);
    
    glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
    
    // Restore state
    glDisable(GL_POLYGON_OFFSET_LINE);
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_CULL_FACE);
    
    glDisableVertexAttribArray(0);
    m_vertexArray->unbind();
    m_wireframeShader->unuse();
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

void Renderer::setViewMatrix(const glm::mat4& viewMatrix) {
    m_viewMatrix = viewMatrix;
}

void Renderer::setProjectionMatrix(const glm::mat4& projMatrix) {
    m_projMatrix = projMatrix;
}

void Renderer::setCameraPosition(const glm::vec3& position) {
    m_cameraPos = position;
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

bool Renderer::initializeShaders() {
    m_basicShader = std::make_unique<Shader>();
    m_wireframeShader = std::make_unique<Shader>();
    
    // Load basic shader
    if (!m_basicShader->loadFromFile("shaders/basic.vert", "shaders/basic.frag")) {
        // Fallback to embedded shaders if files don't exist
        std::string vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPosition;
            layout (location = 1) in vec3 aNormal;
            uniform mat4 uModel;
            uniform mat4 uView;
            uniform mat4 uProjection;
            uniform mat3 uNormalMatrix;
            out vec3 FragPos;
            out vec3 Normal;
            void main() {
                FragPos = vec3(uModel * vec4(aPosition, 1.0));
                Normal = uNormalMatrix * aNormal;
                gl_Position = uProjection * uView * vec4(FragPos, 1.0);
            }
        )";
        
        std::string fragmentSource = R"(
            #version 330 core
            in vec3 FragPos;
            in vec3 Normal;
            uniform vec3 uColor;
            uniform vec3 uLightPos;
            uniform vec3 uLightColor;
            uniform vec3 uViewPos;
            out vec4 FragColor;
            void main() {
                float ambientStrength = 0.3;
                vec3 ambient = ambientStrength * uLightColor;
                vec3 norm = normalize(Normal);
                vec3 lightDir = normalize(uLightPos - FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * uLightColor;
                float specularStrength = 0.5;
                vec3 viewDir = normalize(uViewPos - FragPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                vec3 specular = specularStrength * spec * uLightColor;
                vec3 result = (ambient + diffuse + specular) * uColor;
                FragColor = vec4(result, 1.0); // Ensure alpha is always 1.0
            }
        )";
        
        if (!m_basicShader->loadFromSource(vertexSource, fragmentSource)) {
            return false;
        }
    }
    
    // Load wireframe shader
    if (!m_wireframeShader->loadFromFile("shaders/wireframe.vert", "shaders/wireframe.frag")) {
        std::string wireVertSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPosition;
            uniform mat4 uModel;
            uniform mat4 uView;
            uniform mat4 uProjection;
            void main() {
                gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
            }
        )";
        
        std::string wireFragSource = R"(
            #version 330 core
            uniform vec3 uColor;
            out vec4 FragColor;
            void main() {
                FragColor = vec4(uColor, 1.0);
            }
        )";
        
        if (!m_wireframeShader->loadFromSource(wireVertSource, wireFragSource)) {
            return false;
        }
    }
    
    return true;
}

void Renderer::setupVertexAttributes() {
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, position));
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, normal));
    glEnableVertexAttribArray(1);
}

