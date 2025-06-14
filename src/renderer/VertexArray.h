#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <GL/glew.h>
#include <vector>

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    // Delete copy constructor and assignment operator
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void bind() const;
    void unbind() const;
    
    void addVertexBuffer(GLuint vbo, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
    void setElementBuffer(GLuint ebo);
    
    GLuint getId() const { return m_id; }
    
private:
    GLuint m_id;
    GLuint m_attributeIndex;
};

#endif
