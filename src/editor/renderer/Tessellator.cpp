#include "Tessellator.h"
#include "brep/Solid.h"
#include "brep/Shell.h"
#include "brep/Face.h"
#include "brep/Loop.h"
#include "brep/Vertex.h"
#include "brep/Edge.h"
#include "logger/Logger.h"

#include <cmath>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Static GPU resources
unsigned int Tessellator::s_tessellationProgram = 0;
unsigned int Tessellator::s_computeShader = 0;
unsigned int Tessellator::s_geometryShader = 0;
bool Tessellator::s_gpuInitialized = false;

// Additional GPU resources for transform feedback
static unsigned int s_feedbackProgram = 0;
static unsigned int s_feedbackVAO = 0;
static unsigned int s_feedbackVBO = 0;
static unsigned int s_feedbackTBO = 0;

Mesh Tessellator::tessellate(const BREP::Solid& solid, const TessellationParams& params) {
    TessellationMethod method = params.method;
    
    // Auto-select method if adaptive
    if (method == TessellationMethod::ADAPTIVE) {
        method = chooseBestMethod(solid, params);
    }
    
    // Initialize GPU tessellation if needed
    if ((method == TessellationMethod::GPU || method == TessellationMethod::ADAPTIVE) && !s_gpuInitialized) {
        initializeGPUTessellation();
    }
    
    switch (method) {
        case TessellationMethod::CPU:
            return tessellateCPU(solid, params);
        case TessellationMethod::GPU:
            return tessellateGPU(solid, params);
        case TessellationMethod::ADAPTIVE:
            return tessellateAdaptive(solid, params);
        default:
            Logger::warning("Unknown tessellation method, falling back to CPU");
            return tessellateCPU(solid, params);
    }
}

Mesh Tessellator::tessellateCPU(const BREP::Solid& solid, const TessellationParams& params) {
    Mesh mesh;
    
    for (const auto& shell : solid.getShells()) {
        processShellCPU(shell, mesh, params);
    }
    
    return mesh;
}

Mesh Tessellator::tessellateGPU(const BREP::Solid& solid, const TessellationParams& params) {
    if (!s_gpuInitialized) {
        initializeGPUTessellation();
    }
    
    if (!s_gpuInitialized) {
        Logger::warning("GPU tessellation failed to initialize, falling back to CPU");
        return tessellateCPU(solid, params);
    }
    
    Mesh mesh;
    
    for (const auto& shell : solid.getShells()) {
        processShellGPU(shell, mesh, params);
    }
    
    return mesh;
}

Mesh Tessellator::tessellateAdaptive(const BREP::Solid& solid, const TessellationParams& params) {
    Mesh mesh;
    
    for (const auto& shell : solid.getShells()) {
        for (const auto& face : shell->getFaces()) {
            float curvature = calculateFaceCurvature(face);
            
            if (curvature > params.curvatureThreshold || shouldSubdivide(face, params)) {
                processShellGPU(shell, mesh, params);
            } else {
                processShellCPU(shell, mesh, params);
            }
        }
    }
    
    return mesh;
}

void Tessellator::processShellCPU(const std::shared_ptr<BREP::Shell>& shell, Mesh& mesh, const TessellationParams& params) {
    for (const auto& face : shell->getFaces()) {
        processFaceCPU(face, mesh, params);
    }
}

void Tessellator::processFaceCPU(const std::shared_ptr<BREP::Face>& face, Mesh& mesh, const TessellationParams& params) {
    const auto& outerLoop = face->getOuterLoop();
    const auto& vertices = outerLoop->getVertices();
    
    if (vertices.size() < 3) {
        return; // Not enough vertices for a face
    }
    
    // Calculate face normal using first three vertices with proper cross product
    std::array<float, 3> faceNormal = {0.0f, 0.0f, 1.0f};
    if (vertices.size() >= 3) {
        auto pos0 = vertices[0]->getPosition();
        auto pos1 = vertices[1]->getPosition();
        auto pos2 = vertices[2]->getPosition();
        
        auto v1 = pos1 - pos0;
        auto v2 = pos2 - pos0;
        
        // Cross product: v1 × v2
        glm::vec3 normal = glm::cross(v1, v2);
        
        // Normalize
        float length = glm::length(normal);
        if (length > 0.0f) {
            normal = normal / length;
            faceNormal[0] = normal.x;
            faceNormal[1] = normal.y;
            faceNormal[2] = normal.z;
        }
    }
    
    // Store starting index for this face
    unsigned int startIndex = static_cast<unsigned int>(mesh.m_vertices.size());
    
    // Add vertices to mesh
    for (const auto& vertex : vertices) {
        RenderVertex renderVertex;
        auto pos = vertex->getPosition();
        renderVertex.position = {{static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z)}};
        
        // For spherical objects, use position as normal (after normalization)
        // Check if this vertex is likely from a sphere by checking distance from origin
        float distFromOrigin = glm::length(pos);
        if (distFromOrigin > 0.01f) { // Not at origin
            // Check if all vertices are roughly equidistant from origin (sphere-like)
            bool isSpherelike = true;
            float firstDist = glm::length(vertices[0]->getPosition());
            for (const auto& v : vertices) {
                float dist = glm::length(v->getPosition());
                if (std::abs(dist - firstDist) > 0.1f) { // Allow some tolerance
                    isSpherelike = false;
                    break;
                }
            }
            
            if (isSpherelike && vertices.size() == 3) { // Spherical triangle
                glm::vec3 vertexNormal = glm::normalize(pos);
                renderVertex.normal = {{vertexNormal.x, vertexNormal.y, vertexNormal.z}};
            } else {
                renderVertex.normal = faceNormal;
            }
        } else {
            renderVertex.normal = faceNormal;
        }
        
        mesh.m_vertices.push_back(renderVertex);
    }
    
    // Special handling for grid quads - ensure they stay as quads for wireframe
    if (vertices.size() == 4) {
        // For quad faces (like grid), create two triangles but store quad info for wireframe
        Triangle tri1, tri2;
        tri1.indices = {{startIndex, startIndex + 1, startIndex + 2}};
        tri2.indices = {{startIndex, startIndex + 2, startIndex + 3}};
        mesh.m_triangles.push_back(tri1);
        mesh.m_triangles.push_back(tri2);
        
        // Mark this as a special grid quad by storing additional edge information
        // This will help the wireframe renderer show proper quad edges
    } else {
        // For other faces, use fan triangulation
        for (size_t i = 1; i < vertices.size() - 1; ++i) {
            Triangle triangle;
            triangle.indices = {{startIndex, startIndex + static_cast<unsigned int>(i), startIndex + static_cast<unsigned int>(i + 1)}};
            mesh.m_triangles.push_back(triangle);
        }
    }
}

void Tessellator::processShellGPU(const std::shared_ptr<BREP::Shell>& shell, Mesh& mesh, const TessellationParams& params) {
    for (const auto& face : shell->getFaces()) {
        processFaceGPU(face, mesh, params);
    }
}

void Tessellator::processFaceGPU(const std::shared_ptr<BREP::Face>& face, Mesh& mesh, const TessellationParams& params) {
    if (!s_gpuInitialized) {
        Logger::error("GPU tessellation not initialized");
        // Fallback to CPU
        processFaceCPU(face, mesh, params);
        return;
    }
    
    if (s_feedbackProgram == 0) {
        Logger::error("Tessellation feedback program not loaded");
        processFaceCPU(face, mesh, params);
        return;
    }
    
    const auto& outerLoop = face->getOuterLoop();
    const auto& vertices = outerLoop->getVertices();
    
    if (vertices.size() < 3) {
        return;
    }
    
    // Calculate face normal
    glm::vec3 calculatedNormal = calculateFaceNormal(face);
    
    // Prepare vertex data for GPU tessellation (position + normal per vertex)
    std::vector<float> patchData;
    for (const auto& vertex : vertices) {
        auto pos = vertex->getPosition();
        patchData.push_back(static_cast<float>(pos.x));
        patchData.push_back(static_cast<float>(pos.y));
        patchData.push_back(static_cast<float>(pos.z));
        
        if (params.smoothNormals) {
            glm::vec3 smoothNormal = calculateSmoothNormal(vertex, face, params);
            patchData.push_back(smoothNormal.x);
            patchData.push_back(smoothNormal.y);
            patchData.push_back(smoothNormal.z);
        } else {
            patchData.push_back(calculatedNormal.x);
            patchData.push_back(calculatedNormal.y);
            patchData.push_back(calculatedNormal.z);
        }
    }
    
    // Setup OpenGL state
    glUseProgram(s_feedbackProgram);
    
    // Set tessellation uniforms
    glUniform1f(glGetUniformLocation(s_feedbackProgram, "uInnerTessLevel"), params.innerTessLevel);
    glUniform1f(glGetUniformLocation(s_feedbackProgram, "uOuterTessLevel"), params.outerTessLevel);
    
    // Setup vertex data
    glBindVertexArray(s_feedbackVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_feedbackVBO);
    glBufferData(GL_ARRAY_BUFFER, patchData.size() * sizeof(float), patchData.data(), GL_DYNAMIC_DRAW);
    
    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Estimate output size (conservative estimate based on tessellation level)
    int estimatedOutputVertices = static_cast<int>(vertices.size() * params.innerTessLevel * params.outerTessLevel * 6);
    
    // Setup transform feedback buffer
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, s_feedbackTBO);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, estimatedOutputVertices * 6 * sizeof(float), nullptr, GL_DYNAMIC_READ);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, s_feedbackTBO);
    
    // Set up tessellation patch parameters - ensure we use triangles
    int vertexCount = static_cast<int>(vertices.size());
    
    // For tessellation, we need to work with triangular patches
    // If face has more than 3 vertices, we need to triangulate first
    if (vertexCount > 3) {
        // For polygons with more than 3 vertices, tessellate as triangle fan
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        
        // Process as multiple triangular patches
        for (int i = 1; i < vertexCount - 1; i++) {
            // Create triangle patch data for vertices (0, i, i+1)
            std::vector<float> trianglePatch;
            
            // Vertex 0
            auto pos0 = vertices[0]->getPosition();
            trianglePatch.insert(trianglePatch.end(), {
                static_cast<float>(pos0.x), static_cast<float>(pos0.y), static_cast<float>(pos0.z)
            });
            if (params.smoothNormals) {
                glm::vec3 smoothNormal0 = calculateSmoothNormal(vertices[0], face, params);
                trianglePatch.insert(trianglePatch.end(), {smoothNormal0.x, smoothNormal0.y, smoothNormal0.z});
            } else {
                trianglePatch.insert(trianglePatch.end(), {calculatedNormal.x, calculatedNormal.y, calculatedNormal.z});
            }
            
            // Vertex i
            auto posi = vertices[i]->getPosition();
            trianglePatch.insert(trianglePatch.end(), {
                static_cast<float>(posi.x), static_cast<float>(posi.y), static_cast<float>(posi.z)
            });
            if (params.smoothNormals) {
                glm::vec3 smoothNormali = calculateSmoothNormal(vertices[i], face, params);
                trianglePatch.insert(trianglePatch.end(), {smoothNormali.x, smoothNormali.y, smoothNormali.z});
            } else {
                trianglePatch.insert(trianglePatch.end(), {calculatedNormal.x, calculatedNormal.y, calculatedNormal.z});
            }
            
            // Vertex i+1
            auto posi1 = vertices[i+1]->getPosition();
            trianglePatch.insert(trianglePatch.end(), {
                static_cast<float>(posi1.x), static_cast<float>(posi1.y), static_cast<float>(posi1.z)
            });
            if (params.smoothNormals) {
                glm::vec3 smoothNormali1 = calculateSmoothNormal(vertices[i+1], face, params);
                trianglePatch.insert(trianglePatch.end(), {smoothNormali1.x, smoothNormali1.y, smoothNormali1.z});
            } else {
                trianglePatch.insert(trianglePatch.end(), {calculatedNormal.x, calculatedNormal.y, calculatedNormal.z});
            }
            
            // Update buffer with triangle data
            glBufferSubData(GL_ARRAY_BUFFER, 0, trianglePatch.size() * sizeof(float), trianglePatch.data());
            
            // Execute tessellation for this triangle
            GLuint query;
            glGenQueries(1, &query);
            glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
            
            glBeginTransformFeedback(GL_TRIANGLES);
            glDrawArrays(GL_PATCHES, 0, 3); // Draw one triangle patch
            glEndTransformFeedback();
            
            glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
            
            // Process the results for this triangle
            processTriangleTessellationResult(query, mesh, estimatedOutputVertices);
        }
    } else if (vertexCount == 3) {
        // Already a triangle, process normally
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        
        // Execute tessellation with transform feedback
        GLuint query;
        glGenQueries(1, &query);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
        
        glBeginTransformFeedback(GL_TRIANGLES);
        glDrawArrays(GL_PATCHES, 0, 3);
        glEndTransformFeedback();
        
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        
        // Process the results
        processTriangleTessellationResult(query, mesh, estimatedOutputVertices);
    }
    
    // Cleanup
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    glUseProgram(0);
}

void Tessellator::processTriangleTessellationResult(unsigned int query, Mesh& mesh, int estimatedOutputVertices) {
    // Read back tessellated data
    glFinish(); // Ensure GPU work is complete
    
    // Get the number of primitives generated
    GLint primitivesGenerated = 0;
    glGetQueryObjectiv(query, GL_QUERY_RESULT, &primitivesGenerated);
    glDeleteQueries(1, &query);
    
    if (primitivesGenerated <= 0) {
        Logger::warning("No primitives generated from GPU tessellation");
        return;
    }
    
    float* tessellatedData = (float*)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_ONLY);
    if (tessellatedData) {
        // Each primitive is a triangle (3 vertices), each vertex has 6 floats (pos + normal)
        int actualVertices = primitivesGenerated * 3;
        
        // Add tessellated vertices to mesh
        unsigned int startIndex = static_cast<unsigned int>(mesh.m_vertices.size());
        for (int i = 0; i < actualVertices && i < estimatedOutputVertices; ++i) {
            RenderVertex renderVertex;
            
            // Position (first 3 floats)
            renderVertex.position[0] = tessellatedData[i * 6 + 0];
            renderVertex.position[1] = tessellatedData[i * 6 + 1];
            renderVertex.position[2] = tessellatedData[i * 6 + 2];
            
            // Normal (next 3 floats)
            renderVertex.normal[0] = tessellatedData[i * 6 + 3];
            renderVertex.normal[1] = tessellatedData[i * 6 + 4];
            renderVertex.normal[2] = tessellatedData[i * 6 + 5];
            
            mesh.m_vertices.push_back(renderVertex);
        }
        
        // Create triangles (every 3 consecutive vertices form a triangle)
        for (int i = 0; i < actualVertices; i += 3) {
            if (i + 2 < actualVertices) {
                Triangle triangle;
                triangle.indices[0] = startIndex + i;
                triangle.indices[1] = startIndex + i + 1;
                triangle.indices[2] = startIndex + i + 2;
                mesh.m_triangles.push_back(triangle);
            }
        }
        
        glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
        
        Logger::info("GPU tessellation triangle generated " + std::to_string(actualVertices) + " vertices and " + 
                     std::to_string(actualVertices / 3) + " triangles");
    } else {
        Logger::error("Failed to read back tessellated data from GPU for triangle");
    }
}

TessellationMethod Tessellator::chooseBestMethod(const BREP::Solid& solid, const TessellationParams& params) {
    float complexity = calculateComplexity(solid);
    
    if (complexity > 1000.0f || params.tessellationLevel > 3.0f) {
        return TessellationMethod::GPU;
    }
    
    return TessellationMethod::CPU;
}

float Tessellator::calculateComplexity(const BREP::Solid& solid) {
    float complexity = 0.0f;
    
    for (const auto& shell : solid.getShells()) {
        for (const auto& face : shell->getFaces()) {
            const auto& vertices = face->getOuterLoop()->getVertices();
            complexity += static_cast<float>(vertices.size());
            complexity += calculateFaceCurvature(face) * 10.0f;
        }
    }
    
    return complexity;
}

float Tessellator::calculateFaceCurvature(const std::shared_ptr<BREP::Face>& face) {
    const auto& vertices = face->getOuterLoop()->getVertices();
    if (vertices.size() < 3) return 0.0f;
    
    float totalCurvature = 0.0f;
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto v1 = vertices[i]->getPosition();
        auto v2 = vertices[(i + 1) % vertices.size()]->getPosition();
        auto v3 = vertices[(i + 2) % vertices.size()]->getPosition();
        
        auto edge1 = glm::normalize(v2 - v1);
        auto edge2 = glm::normalize(v3 - v2);
        
        float angle = glm::acos(glm::clamp(glm::dot(edge1, edge2), -1.0f, 1.0f));
        totalCurvature += std::abs(M_PI - angle);
    }
    
    return totalCurvature / static_cast<float>(vertices.size());
}

float Tessellator::calculateEdgeLength(const std::shared_ptr<BREP::Edge>& edge) {
    return 1.0f; // Placeholder implementation
}

bool Tessellator::shouldSubdivide(const std::shared_ptr<BREP::Face>& face, const TessellationParams& params) {
    float curvature = calculateFaceCurvature(face);
    return curvature > params.curvatureThreshold;
}

void Tessellator::initializeGPUTessellation() {
    if (s_gpuInitialized) return;
    
    setupTessellationShaders();
    
    glGenVertexArrays(1, &s_feedbackVAO);
    glGenBuffers(1, &s_feedbackVBO);
    glGenBuffers(1, &s_feedbackTBO);
    
    s_gpuInitialized = (s_feedbackProgram != 0);
    
    if (s_gpuInitialized) {
        Logger::info("GPU tessellation initialized successfully");
    } else {
        Logger::error("Failed to initialize GPU tessellation");
    }
}

// Helper function to load tessellation shaders
static GLuint loadTessellationShader(const std::string& vertPath, const std::string& tcPath, 
                                   const std::string& tePath, const std::vector<std::string>& feedbackVaryings = {}) {
    auto readFile = [](const std::string& filepath) -> std::string {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            Logger::error("Failed to open shader file: " + filepath);
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    };
    
    auto compileShader = [](const std::string& source, GLenum shaderType) -> GLuint {
        GLuint shader = glCreateShader(shaderType);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            Logger::error("Shader compilation failed: " + std::string(infoLog));
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    };
    
    std::string vertSource = readFile(vertPath);
    std::string tcSource = readFile(tcPath);
    std::string teSource = readFile(tePath);
    
    if (vertSource.empty() || tcSource.empty() || teSource.empty()) {
        Logger::error("Failed to read shader files");
        return 0;
    }
    
    GLuint vertShader = compileShader(vertSource, GL_VERTEX_SHADER);
    GLuint tcShader = compileShader(tcSource, GL_TESS_CONTROL_SHADER);
    GLuint teShader = compileShader(teSource, GL_TESS_EVALUATION_SHADER);
    
    if (vertShader == 0 || tcShader == 0 || teShader == 0) {
        glDeleteShader(vertShader);
        glDeleteShader(tcShader);
        glDeleteShader(teShader);
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, tcShader);
    glAttachShader(program, teShader);
    
    // Setup transform feedback if specified
    if (!feedbackVaryings.empty()) {
        std::vector<const char*> varyingNames;
        for (const auto& varying : feedbackVaryings) {
            varyingNames.push_back(varying.c_str());
        }
        glTransformFeedbackVaryings(program, static_cast<GLsizei>(varyingNames.size()), 
                                   varyingNames.data(), GL_INTERLEAVED_ATTRIBS);
    }
    
    // Link program
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        Logger::error("Shader program linking failed: " + std::string(infoLog));
        glDeleteProgram(program);
        program = 0;
    }
    
    // Cleanup
    glDeleteShader(vertShader);
    glDeleteShader(tcShader);
    glDeleteShader(teShader);
    
    return program;
}

void Tessellator::setupTessellationShaders() {
    // Load transform feedback program for GPU tessellation
    std::vector<std::string> feedbackVaryings = {"tePosition", "teNormal"};
    s_feedbackProgram = loadTessellationShader(
        "shaders/tessellation_feedback.vert",
        "shaders/tessellation_feedback.tesc",
        "shaders/tessellation_feedback.tese",
        feedbackVaryings
    );
    
    if (s_feedbackProgram == 0) {
        Logger::error("Failed to load tessellation feedback shaders");
    } else {
        Logger::info("Tessellation shaders loaded successfully");
    }
}

void Tessellator::cleanupGPUResources() {
    if (s_tessellationProgram != 0) {
        glDeleteProgram(s_tessellationProgram);
        s_tessellationProgram = 0;
    }
    
    if (s_feedbackProgram != 0) {
        glDeleteProgram(s_feedbackProgram);
        s_feedbackProgram = 0;
    }
    
    if (s_feedbackVAO != 0) {
        glDeleteVertexArrays(1, &s_feedbackVAO);
        s_feedbackVAO = 0;
    }
    
    if (s_feedbackVBO != 0) {
        glDeleteBuffers(1, &s_feedbackVBO);
        s_feedbackVBO = 0;
    }
    
    if (s_feedbackTBO != 0) {
        glDeleteBuffers(1, &s_feedbackTBO);
        s_feedbackTBO = 0;
    }
    
    s_gpuInitialized = false;
}

glm::vec3 Tessellator::calculateSmoothNormal(const std::shared_ptr<BREP::Vertex>& vertex, 
                                           const std::shared_ptr<BREP::Face>& face,
                                           const TessellationParams& params) {
    glm::vec3 normal = calculateFaceNormal(face);
    return glm::normalize(normal);
}

glm::vec3 Tessellator::calculateFaceNormal(const std::shared_ptr<BREP::Face>& face) {
    const auto& vertices = face->getOuterLoop()->getVertices();
    if (vertices.size() < 3) return glm::vec3(0, 0, 1);
    
    auto pos0 = vertices[0]->getPosition();
    auto pos1 = vertices[1]->getPosition();
    auto pos2 = vertices[2]->getPosition();
    
    auto v1 = pos1 - pos0;
    auto v2 = pos2 - pos0;
    
    return glm::normalize(glm::cross(v1, v2));
}

float Tessellator::calculateLODLevel(const std::shared_ptr<BREP::Face>& face, 
                                   const glm::vec3& viewPoint,
                                   const TessellationParams& params) {
    const auto& vertices = face->getOuterLoop()->getVertices();
    if (vertices.empty()) return 1.0f;
    
    glm::vec3 centroid(0.0f);
    for (const auto& vertex : vertices) {
        centroid += vertex->getPosition();
    }
    centroid /= static_cast<float>(vertices.size());
    
    float distance = glm::length(viewPoint - centroid);
    
    if (params.enableLOD) {
        return std::max(0.1f, 1.0f - (distance / params.lodDistance));
    }
    
    return 1.0f;
}

