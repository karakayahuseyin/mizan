#include "VertexArray.h"

VertexArray::VertexArray() : m_id(0), m_attributeIndex(0) {
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() {
    if (m_id != 0) {
        glDeleteVertexArrays(1, &m_id);
    }
}

void VertexArray::bind() const {
    glBindVertexArray(m_id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(GLuint vbo, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(m_attributeIndex, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(m_attributeIndex);
    m_attributeIndex++;
}

void VertexArray::setElementBuffer(GLuint ebo) {
    bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}
