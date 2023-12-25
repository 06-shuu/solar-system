#include "IBO.h"

IBO::IBO() {
	glGenBuffers(1, &ID);
}

IBO::~IBO() {
	glDeleteBuffers(1, &ID);
}

void IBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void IBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IBO::BufferData(GLsizeiptr size, GLuint* indices) {
	//bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}