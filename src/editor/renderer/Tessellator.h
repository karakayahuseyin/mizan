#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include "brep/Types.h"
#include "Mesh.h"

#include <memory>
#include <glm/glm.hpp>

enum class TessellationMethod {
    CPU,
    GPU,
    ADAPTIVE  // Automatically choose based on geometry complexity
};

struct TessellationParams {
    TessellationMethod method = TessellationMethod::CPU;
    
    // Quality parameters
    float tessellationLevel = 1.0f;        // Base tessellation density (0.1 - 10.0)
    float curvatureThreshold = 0.1f;       // Curvature sensitivity for adaptive tessellation
    float edgeLengthLimit = 1.0f;          // Maximum edge length before subdivision
    
    // GPU-specific parameters
    float innerTessLevel = 4.0f;           // GPU tessellation inner level
    float outerTessLevel = 4.0f;           // GPU tessellation outer level
    bool useGeometryShader = true;         // Enable geometry shader for complex primitives
    
    // Performance parameters
    int maxVerticesPerPatch = 32;          // Limit vertices per tessellation patch
    bool enableLOD = false;                // Level of detail based on distance
    float lodDistance = 10.0f;             // Distance threshold for LOD
    
    // Surface-specific parameters
    bool smoothNormals = true;             // Calculate smooth vertex normals
    bool preserveSharpEdges = true;        // Maintain sharp edges during tessellation
    float sharpEdgeThreshold = 60.0f;      // Angle threshold for sharp edges (degrees)
};

class Tessellator {
public:
    // Main tessellation entry point with parameters
    static Mesh tessellate(const BREP::Solid& solid, const TessellationParams& params);
    
    // Compatibility overload for backward compatibility
    static Mesh tessellate(const BREP::Solid& solid) {
        return tessellate(solid, TessellationParams{});
    }
    
    // Specific method tessellation functions
    static Mesh tessellateCPU(const BREP::Solid& solid, const TessellationParams& params);
    static Mesh tessellateGPU(const BREP::Solid& solid, const TessellationParams& params);
    static Mesh tessellateAdaptive(const BREP::Solid& solid, const TessellationParams& params);
    
    // Utility functions
    static TessellationMethod chooseBestMethod(const BREP::Solid& solid, const TessellationParams& params);
    static float calculateComplexity(const BREP::Solid& solid);
    
    // GPU compute shader setup
    static void initializeGPUTessellation();
    static void setupTessellationShaders();
    static void cleanupGPUResources();
    static bool isGPUInitialized() { return s_gpuInitialized; }
    
private:
    // CPU tessellation methods
    static void processShellCPU(const std::shared_ptr<BREP::Shell>& shell, Mesh& mesh, const TessellationParams& params);
    static void processFaceCPU(const std::shared_ptr<BREP::Face>& face, Mesh& mesh, const TessellationParams& params);
    
    // GPU tessellation methods
    static void processShellGPU(const std::shared_ptr<BREP::Shell>& shell, Mesh& mesh, const TessellationParams& params);
    static void processFaceGPU(const std::shared_ptr<BREP::Face>& face, Mesh& mesh, const TessellationParams& params);
    static void processTriangleTessellationResult(unsigned int query, Mesh& mesh, int estimatedOutputVertices);
    
    // Adaptive tessellation helpers
    static float calculateFaceCurvature(const std::shared_ptr<BREP::Face>& face);
    static float calculateEdgeLength(const std::shared_ptr<BREP::Edge>& edge);
    static bool shouldSubdivide(const std::shared_ptr<BREP::Face>& face, const TessellationParams& params);
    
    // Normal calculation helpers
    static glm::vec3 calculateSmoothNormal(const std::shared_ptr<BREP::Vertex>& vertex, 
                                         const std::shared_ptr<BREP::Face>& face,
                                         const TessellationParams& params);
    static glm::vec3 calculateFaceNormal(const std::shared_ptr<BREP::Face>& face);
    
    // LOD helpers
    static float calculateLODLevel(const std::shared_ptr<BREP::Face>& face, 
                                 const glm::vec3& viewPoint,
                                 const TessellationParams& params);
    
    // GPU resources
    static unsigned int s_tessellationProgram;
    static unsigned int s_computeShader;
    static unsigned int s_geometryShader;
    static bool s_gpuInitialized;
};

#endif