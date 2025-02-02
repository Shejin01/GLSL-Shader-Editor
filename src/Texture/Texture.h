#pragma once

#include <glad/glad.h>
#include "stb_image.h"
#include "../Logger/Logger.h"
#include "../Math/Precision.h"

class Texture {
private:
	int32	width, height, nrChannels;
	String	texturePath = "";
	void CreateTexture(unsigned char* pixels, int32 width, int32 height, int32 nrChannels);
public:
	uint32 ID = 0;

	Texture();
	Texture(unsigned char* pixels, int32 width, int32 height, int32 nrChannels);
	Texture(const char* texturePath, bool* success = nullptr);

	int32	GetWidth()				const;
	int32	GetHeight()				const;
	int32	GetNumberOfChannels()	const;
	String	GetTexturePath()		const;

	void Bind(uint32 textureUnit);
	void Unbind();
	void Delete();
};