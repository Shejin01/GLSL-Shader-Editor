#pragma once

#include <glad/glad.h>
#include "stb_image.h"
#include <vector>
#include "../Logger/Logger.h"
#include "../Math/Precision.h"

class Cubemap {
	std::vector<String> texturePaths;
public:
	uint32 ID = 0;

	Cubemap();
	Cubemap(std::vector<String> texturePaths, bool* success = nullptr);
	
	std::vector<String> GetTexturePaths() const;

	void Bind(uint32 textureUnit);
	void Unbind();
	void Delete();
};