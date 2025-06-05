#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	// Assigns the type of the texture ot the texture object
	type = texType;

	// Stores the width, height, and the number of color channels of the image
	int widthImg = 0, heightImg = 0, numColCh = 0;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	std::cout << "[Texture] Loading: " << image << std::endl;
	std::cout << "[Texture] stbi_load returned: " << (void*)bytes << ", width: " << widthImg << ", height: " << heightImg << ", channels: " << numColCh << std::endl;

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	std::cout << "[Texture] OpenGL texture ID: " << ID << std::endl;
	// Assigns the texture to a Texture Unit
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum imgFormat = GL_RGBA;
	GLenum imgInternalFormat = GL_RGBA;
	if (numColCh == 1) {
		imgFormat = GL_RED;
		imgInternalFormat = GL_RED;
	} else if (numColCh == 3) {
		imgFormat = GL_RGB;
		imgInternalFormat = GL_RGB;
	} else if (numColCh == 4) {
		imgFormat = GL_RGBA;
		imgInternalFormat = GL_RGBA;
	} else {
		std::cout << "[Texture] WARNING: Unexpected channel count (" << numColCh << "). Defaulting to RGBA." << std::endl;
	}

	if (!bytes || widthImg <= 0 || heightImg <= 0) {
		std::cout << "[Texture] ERROR: Failed to load image or invalid dimensions. Using fallback 1x1 white texture for: " << image << std::endl;
		unsigned char whitePixel[] = {255, 255, 255, 255};
		glTexImage2D(texType, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
	} else {
		std::cout << "[Texture] SUCCESS: Image loaded. Dimensions: " << widthImg << "x" << heightImg << ", Channels: " << numColCh << std::endl;
		// Assigns the image to the OpenGL Texture object
		glTexImage2D(texType, 0, imgInternalFormat, widthImg, heightImg, 0, imgFormat, pixelType, bytes);
		stbi_image_free(bytes);
	}
	// Generates MipMaps
	glGenerateMipmap(texType);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);
}

void Texture::texUnit(const std::shared_ptr<Shader> &shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader->ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader->use();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}