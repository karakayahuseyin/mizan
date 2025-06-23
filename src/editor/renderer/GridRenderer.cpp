#include "GridRenderer.h"
#include "Shader.h"
#include "logger/Logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GridRenderer::GridRenderer() {
}

GridRenderer::~GridRenderer() {
    cleanup();
}

bool GridRenderer::initialize() {
    if (!createShader()) {
        Logger::error("Failed to create grid shader");
        return false;
    }
    
    // Generate VAO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    
    if (m_vao == 0 || m_vbo == 0) {
        Logger::error("Failed to generate grid OpenGL objects");
        return false;
    }
    
    generateGridVertices();
    updateBuffers();
    
    Logger::info("Grid renderer initialized successfully");
    return true;
}

void GridRenderer::cleanup() {
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    
    m_gridShader.reset();
}

void GridRenderer::render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
    if (!m_gridShader || m_vao == 0) {
        return;
    }
    
    if (m_needsUpdate) {
        generateGridVertices();
        updateBuffers();
        m_needsUpdate = false;
    }
    
    // Use grid shader
    m_gridShader->use();
    
    // Set uniforms
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    
    m_gridShader->setUniform("u_MVP", mvp);
    m_gridShader->setUniform("u_GridColor", m_gridColor);
    
    // Bind VAO and render
    glBindVertexArray(m_vao);
    
    // Disable depth writing for grid (so it appears behind objects)
    glDepthMask(GL_FALSE);
    
    // Render grid lines
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices.size()));
    
    // Re-enable depth writing
    glDepthMask(GL_TRUE);
    
    glBindVertexArray(0);
    m_gridShader->unuse();
}

void GridRenderer::setGridSize(float size) {
    if (m_gridSize != size) {
        m_gridSize = size;
        m_needsUpdate = true;
    }
}

void GridRenderer::setGridSpacing(float spacing) {
    if (m_gridSpacing != spacing) {
        m_gridSpacing = spacing;
        m_needsUpdate = true;
    }
}

void GridRenderer::setGridColor(const glm::vec3& color) {
    m_gridColor = color;
}

void GridRenderer::regenerateGrid() {
    m_needsUpdate = true;
}

void GridRenderer::generateGridVertices() {
    m_vertices.clear();
    
    float halfSize = m_gridSize * 0.5f;
    int numLines = static_cast<int>(m_gridSize / m_gridSpacing) + 1;
    
    // Generate lines parallel to X-axis (running along Z direction)
    for (int i = 0; i < numLines; ++i) {
        float z = -halfSize + i * m_gridSpacing;
        m_vertices.emplace_back(-halfSize, 0.0f, z);
        m_vertices.emplace_back(halfSize, 0.0f, z);
    }
    
    // Generate lines parallel to Z-axis (running along X direction)
    for (int i = 0; i < numLines; ++i) {
        float x = -halfSize + i * m_gridSpacing;
        m_vertices.emplace_back(x, 0.0f, -halfSize);
        m_vertices.emplace_back(x, 0.0f, halfSize);
    }
}

void GridRenderer::updateBuffers() {
    if (m_vao == 0 || m_vbo == 0) {
        return;
    }
    
    glBindVertexArray(m_vao);
    
    // Update vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 m_vertices.size() * sizeof(glm::vec3), 
                 m_vertices.data(), 
                 GL_STATIC_DRAW);
    
    // Setup vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    
    glBindVertexArray(0);
}

bool GridRenderer::createShader() {
    // Grid vertex shader - simple pass-through with MVP transformation
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 u_MVP;
        
        void main() {
            gl_Position = u_MVP * vec4(aPos, 1.0);
        }
    )";
    
    // Grid fragment shader - simple color output
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        uniform vec3 u_GridColor;
        
        void main() {
            FragColor = vec4(u_GridColor, 1.0);
        }
    )";
    
    try {
        m_gridShader = std::make_unique<Shader>();
        if (!m_gridShader->loadFromSource(vertexShaderSource, fragmentShaderSource)) {
            Logger::error("Failed to compile grid shader");
            m_gridShader.reset();
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        Logger::error("Exception while creating grid shader: " + std::string(e.what()));
        m_gridShader.reset();
        return false;
    }
}
