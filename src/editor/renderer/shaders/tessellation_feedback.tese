#version 410 core

layout (triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec3 tcNormal[];

out vec3 tePosition;
out vec3 teNormal;

// Check if geometry appears to be a sphere (all vertices equidistant from center)
bool isSphereGeometry() {
    vec3 center = (tcPosition[0] + tcPosition[1] + tcPosition[2]) / 3.0;
    float dist0 = length(tcPosition[0] - center);
    float dist1 = length(tcPosition[1] - center);
    float dist2 = length(tcPosition[2] - center);
    
    float avgDist = (dist0 + dist1 + dist2) / 3.0;
    float tolerance = 0.1;
    
    // Check if all distances are roughly equal (sphere-like)
    return (abs(dist0 - avgDist) < tolerance && 
            abs(dist1 - avgDist) < tolerance && 
            abs(dist2 - avgDist) < tolerance &&
            avgDist > 0.1);
}

// Check if geometry appears to be planar (flat surface)
bool isPlanarGeometry() {
    vec3 v1 = tcPosition[1] - tcPosition[0];
    vec3 v2 = tcPosition[2] - tcPosition[0];
    vec3 normal = normalize(cross(v1, v2));
    
    // Check if all vertices lie roughly on the same plane
    float tolerance = 0.05;
    for (int i = 0; i < 3; i++) {
        vec3 toVertex = tcPosition[i] - tcPosition[0];
        float distance = abs(dot(toVertex, normal));
        if (distance > tolerance) {
            return false;
        }
    }
    return true;
}

// Apply appropriate surface displacement based on geometry type
vec3 applySurfaceDisplacement(vec3 pos) {
    if (isSphereGeometry()) {
        // For spherical geometry, project to sphere surface
        vec3 center = (tcPosition[0] + tcPosition[1] + tcPosition[2]) / 3.0;
        float avgRadius = (length(tcPosition[0] - center) + 
                          length(tcPosition[1] - center) + 
                          length(tcPosition[2] - center)) / 3.0;
        if (avgRadius > 0.1) {
            return center + normalize(pos - center) * avgRadius;
        }
    } else if (isPlanarGeometry()) {
        // For planar geometry, keep vertices on the plane
        return pos; // No displacement for flat surfaces
    } else {
        // For cylindrical or other curved geometry, apply minimal smoothing
        return pos;
    }
    
    return pos;
}

void main() {
    vec3 bary = gl_TessCoord;
    
    // Interpolate position using barycentric coordinates
    vec3 tessellatedPos = bary.x * tcPosition[0] + 
                         bary.y * tcPosition[1] + 
                         bary.z * tcPosition[2];
    
    // Apply geometry-aware surface displacement
    tessellatedPos = applySurfaceDisplacement(tessellatedPos);
    
    // Interpolate normal
    vec3 normal = bary.x * tcNormal[0] + 
                  bary.y * tcNormal[1] + 
                  bary.z * tcNormal[2];
    
    // Calculate appropriate normal based on geometry type
    if (isSphereGeometry()) {
        // For spheres, use position as normal
        vec3 center = (tcPosition[0] + tcPosition[1] + tcPosition[2]) / 3.0;
        normal = normalize(tessellatedPos - center);
    } else {
        // For other geometry, use interpolated normal
        normal = normalize(normal);
    }
    
    tePosition = tessellatedPos;
    teNormal = normal;
    gl_Position = vec4(tessellatedPos, 1.0);
}
