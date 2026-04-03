#pragma once

#include <string>
#include <assimp/scene.h>

class Texture {
public:
	unsigned int ID;
	int width, height, channels;

	Texture(const char* path);

	Texture(const aiTexture* embeddedTex);
	void Bind(unsigned int slot = 0) const;

	void Unbind() const;
};