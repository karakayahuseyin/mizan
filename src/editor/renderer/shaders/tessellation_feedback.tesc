#version 410 core

layout (vertices = 3) out;

uniform float uInnerTessLevel;
uniform float uOuterTessLevel;

in vec3 vPosition[];
in vec3 vNormal[];

out vec3 tcPosition[];
out vec3 tcNormal[];

void main() {
    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = uInnerTessLevel;
        gl_TessLevelOuter[0] = uOuterTessLevel;
        gl_TessLevelOuter[1] = uOuterTessLevel;
        gl_TessLevelOuter[2] = uOuterTessLevel;
    }
}
