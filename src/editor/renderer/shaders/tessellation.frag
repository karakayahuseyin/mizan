#version 410 core

in vec3 tePosition;
in vec3 teNormal;
in vec3 teWorldPos;

uniform vec3 uColor;
uniform vec3 uLightPosition;
uniform vec3 uCameraPosition;
uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform int uShininess;

out vec4 FragColor;

void main() {
    // Normalize interpolated normal
    vec3 normal = normalize(teNormal);
    
    // Basic Phong lighting
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    // Ambient lighting
    vec3 ambient = uAmbientStrength * lightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(uLightPosition - teWorldPos);
    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;
    
    // Specular lighting
    vec3 viewDir = normalize(uCameraPosition - teWorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularAmount = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = uSpecularStrength * specularAmount * lightColor;
    
    // Combine lighting
    vec3 result = (ambient + diffuse + specular) * uColor;
    
    FragColor = vec4(result, 1.0);
}
