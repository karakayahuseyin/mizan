#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 vPosition;
out vec3 vNormal;

void main() {
    vPosition = aPosition;
    vNormal = aNormal;
    
    // Transform to world space for tessellation control shader
    vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
    gl_Position = worldPos;
}
