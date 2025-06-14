#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();
    
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    void use() const;
    void unuse() const;
    
    // Uniform setters
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, bool value) const;
    
    GLuint getProgramId() const { return m_programId; }
    bool isValid() const { return m_programId != 0; }

private:
    GLuint m_programId;
    
    GLuint compileShader(const std::string& source, GLenum shaderType);
    std::string readFile(const std::string& filepath);
    void checkCompileErrors(GLuint shader, const std::string& type);
    void checkLinkErrors(GLuint program);
    GLint getUniformLocation(const std::string& name) const;
};

#endif
