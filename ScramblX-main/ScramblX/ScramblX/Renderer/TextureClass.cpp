#include "TextureClass.h"

#include <glad/glad.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char* path) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	if (data) {
		GLenum format;

		if (channels == 1) {
			format = GL_RED;
		}
		else if (channels == 3) {
			format = GL_RGB;
		}
		else if (channels == 4) {
			format = GL_RGBA;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			format,
			width,
			height,
			0,
			format,
			GL_UNSIGNED_BYTE,
			data
		);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture: " << path << std::endl;
	}

	stbi_image_free(data);
}

Texture::Texture(const aiTexture* embeddedTex) {
	glGenTextures(1, &ID);
	int width, height, nrComponents;
	unsigned char* data = nullptr;

	if (embeddedTex->mHeight == 0) { // Compressed (png/jpg)
		data = stbi_load_from_memory((unsigned char*)embeddedTex->pcData, embeddedTex->mWidth, &width, &height, &nrComponents, 0);
	}
	else { // Raw data
		data = stbi_load_from_memory((unsigned char*)embeddedTex->pcData, embeddedTex->mWidth * embeddedTex->mHeight, &width, &height, &nrComponents, 0);
	}

	if (data) {
		GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
}

void Texture::Bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}