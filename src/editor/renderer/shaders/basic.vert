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
