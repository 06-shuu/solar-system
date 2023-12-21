#ifndef TETXTURE_H
#define TEXTURE_H

#include  "GL/glew.h"
#include <string>

using namespace std; // to avoide rewriting std::

class Texture {
public:
	Texture(); //constructor
	virtual ~Texture(); //to enable overload (rewrite)
	bool loadTexture(const string &filename, bool generateMipMap);
	//bool loadCubemap(const std::string& folderPath, const std::string& extension); //for skybox
	bool loadCubemap(const std::string& folderPath, const std::string& extension, bool generateMipmaps); //for skybox with mipmap 
	void bind(GLuint texUnit = 0);
	void unbind(GLuint texUnit);
private: 
	GLuint mTexture; //to load multiple textures

};



#endif //