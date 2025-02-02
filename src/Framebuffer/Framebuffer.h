#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Logger/Logger.h"
#include "../Math/Precision.h"
#include "../Texture/Texture.h"

class FBO {
private:
	int32	width, height;
	Texture texture;
	uint32	rbo;
public:
	uint32	ID;

	FBO();
	FBO(int32 width, int32 height);
	uint32	GetTextureID()	const;
	void	Bind();
	void	BindTexture(int32 textureUnit);
	void	Unbind(glm::vec2 defaultSize);
	void	UnbindTexture();
	void	Delete();
};