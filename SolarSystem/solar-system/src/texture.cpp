#include "texture.h"

//texture rendering
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

Texture::Texture():mTexture(0)
{
}


Texture::~Texture()
{
	glDeleteTextures(1, &mTexture);
} 
bool Texture::loadTexture(const string& filename, bool generateMipMap)
{

	//**********************
	// TEXTURE
	// *********************

	/*GLuint texture;*/
	GLint width, height, numComponent;
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	//set parameters
	//wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //S
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //T
	//set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //mag=magnified

	//flipping the image
	stbi_set_flip_vertically_on_load(true);

	//load textture from file
	unsigned char* texData = stbi_load(filename.c_str(), &width, &height, &numComponent, 0);
	
	GLenum format;
	if (numComponent == 1)
		format = GL_RED;
	if (numComponent == 3)
		format = GL_RGB;
	if (numComponent == 4)
		format = GL_RGBA;
	//check for errors
	if (!texData) {
		printf("Failing to load image %s\n", filename.c_str());
	}
	if(generateMipMap)
		glGenerateMipmap(GL_TEXTURE_2D); //optional

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
	stbi_image_free(texData);

	return true;
}

//-----------------------------------------------------------------------------
// (Added to the project) to load cubemap 
//-
bool Texture::loadCubemap(const std::string& folderPath, const std::string& extension, bool generateMipmaps) {
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	const std::string faces[6] = { "right", "left", "top", "bottom", "front", "back" }; //all .jpg

	for (unsigned int i = 0; i < 6; i++) {
		std::string imagePath = folderPath + "/" + faces[i] + extension;
		int width, height, nrChannels;
		unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);

		if (data) {
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			printf("Failing to load image %s\n", imagePath.c_str());
			glDeleteTextures(1, &cubemapTexture);  // Cleanup on failure
			return false;
		}
	}

	if (generateMipmaps) {
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	mTexture = cubemapTexture;

	return true;
}


void Texture::bind(GLuint texUnit) //to control the number of the texture that we want to use
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

//unbind texture method
void Texture::unbind(GLuint texUnit) //to control the number of the texture that we want to use
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, 0); //0 for unbind
}