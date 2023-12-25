#ifndef IBO_H
#define IBO_H

#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"

//using namespace std;

class IBO {
public:
	IBO();
	~IBO();
	void bind();
	void unbind();
	void BufferData(GLsizeiptr size, GLuint* indices);

private:
	GLuint ID;
};

#endif