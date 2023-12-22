#include "VAO.h"

// construct

VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

// deconstruct
VAO::~VAO() {
	glDeleteVertexArrays(1, &ID);
}

void VAO::bind() {
	glBindVertexArray(ID);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::Attribpointer(VAO& VAO, GLuint location, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	VAO.bind();
	glVertexAttribPointer(location, numComponents, type, GL_FALSE, stride * sizeof(GLfloat), offset);
	glEnableVertexAttribArray(location);
}