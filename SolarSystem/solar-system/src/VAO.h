#ifndef VAO_H
#define VAO_H

#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"

//using namespace std;

class VAO {
public:
	VAO();
	void bind();
	void unbind();
	void Attribpointer(VAO& VAO, GLuint location, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

private:
	GLuint ID;
};

#endif