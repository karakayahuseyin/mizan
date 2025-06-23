#version 410 core

layout (triangles, equal_spacing, ccw) in;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform bool uSmoothNormals;
uniform bool uPreserveSharpEdges;
uniform float uSharpEdgeThreshold;

in vec3 tcPosition[];
in vec3 tcNormal[];

out vec3 tePosition;
out vec3 teNormal;
out vec3 teWorldPos;

// Interpolate position using barycentric coordinates
vec3 interpolatePosition(vec3 bary) {
    return bary.x * tcPosition[0] + 
           bary.y * tcPosition[1] + 
           bary.z * tcPosition[2];
}

// Interpolate normal using barycentric coordinates
vec3 interpolateNormal(vec3 bary) {
    vec3 normal = bary.x * tcNormal[0] + 
                  bary.y * tcNormal[1] + 
                  bary.z * tcNormal[2];
    return normalize(normal);
}

// Calculate normal from tessellated position (for displacement mapping)
vec3 calculateTessellatedNormal(vec3 pos, vec3 bary) {
    if (!uSmoothNormals) {
        // Use face normal
        vec3 edge1 = tcPosition[1] - tcPosition[0];
        vec3 edge2 = tcPosition[2] - tcPosition[0];
        return normalize(cross(edge1, edge2));
    }
    
    // Use interpolated smooth normal
    return interpolateNormal(bary);
}

// Apply displacement for spherical surfaces
vec3 applySphericalDisplacement(vec3 pos) {
    // Check if this is likely a sphere by examining the vertices
    vec3 center = (tcPosition[0] + tcPosition[1] + tcPosition[2]) / 3.0;
    float avgRadius = (length(tcPosition[0]) + length(tcPosition[1]) + length(tcPosition[2])) / 3.0;
    
    // If vertices are roughly equidistant from origin, apply spherical displacement
    if (avgRadius > 0.1 && 
        abs(length(tcPosition[0]) - avgRadius) < 0.1 &&
        abs(length(tcPosition[1]) - avgRadius) < 0.1 &&
        abs(length(tcPosition[2]) - avgRadius) < 0.1) {
        return normalize(pos) * avgRadius;
    }
    
    return pos;
}

void main() {
    // Get barycentric coordinates from tessellator
    vec3 bary = gl_TessCoord;
    
    // Interpolate position
    vec3 tessellatedPos = interpolatePosition(bary);
    
    // Apply surface-specific displacement (e.g., for spheres)
    tessellatedPos = applySphericalDisplacement(tessellatedPos);
    
    // Calculate normal
    vec3 normal = calculateTessellatedNormal(tessellatedPos, bary);
    
    // Transform to world space
    vec4 worldPos = uModelMatrix * vec4(tessellatedPos, 1.0);
    
    // Output values
    tePosition = tessellatedPos;
    teWorldPos = worldPos.xyz;
    teNormal = mat3(transpose(inverse(uModelMatrix))) * normal;
    
    // Transform to clip space
    gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
}
