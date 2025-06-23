#version 410 core

layout (vertices = 3) out;

uniform float uInnerTessLevel;
uniform float uOuterTessLevel;
uniform float uTessellationLevel;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPosition;
uniform bool uEnableLOD;
uniform float uLODDistance;

in vec3 vPosition[];
in vec3 vNormal[];

out vec3 tcPosition[];
out vec3 tcNormal[];

// Calculate distance-based LOD
float calculateLOD(vec3 worldPos) {
    if (!uEnableLOD) {
        return uTessellationLevel;
    }
    
    float distance = length(uCameraPosition - worldPos);
    float lodFactor = max(0.1, 1.0 - (distance / uLODDistance));
    return mix(1.0, uTessellationLevel, lodFactor);
}

// Calculate tessellation level based on edge length and curvature
float calculateEdgeTessLevel(vec3 p0, vec3 p1) {
    float edgeLength = length(p1 - p0);
    vec3 midPoint = (p0 + p1) * 0.5;
    float lod = calculateLOD(midPoint);
    
    // Adjust tessellation based on edge length
    float tessLevel = lod * max(1.0, edgeLength * 2.0);
    return clamp(tessLevel, 1.0, 64.0);
}

void main() {
    // Pass through position and normal
    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    // Calculate tessellation levels only for the first invocation
    if (gl_InvocationID == 0) {
        // Calculate adaptive tessellation levels based on edge lengths
        vec3 p0 = vPosition[0];
        vec3 p1 = vPosition[1];
        vec3 p2 = vPosition[2];
        
        float tessLevel0 = calculateEdgeTessLevel(p1, p2); // Edge opposite to vertex 0
        float tessLevel1 = calculateEdgeTessLevel(p2, p0); // Edge opposite to vertex 1
        float tessLevel2 = calculateEdgeTessLevel(p0, p1); // Edge opposite to vertex 2
        
        // Set outer tessellation levels (edges)
        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        
        // Set inner tessellation level (triangle interior)
        float avgTessLevel = (tessLevel0 + tessLevel1 + tessLevel2) / 3.0;
        gl_TessLevelInner[0] = max(uInnerTessLevel, avgTessLevel);
    }
}
