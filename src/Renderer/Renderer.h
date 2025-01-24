#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include "Buffers/VAO.h"
#include "Buffers/VBO.h"

class Renderer {
private:
	VBO quadVbo;
	VAO quadVao;
public:
	void Clear(glm::vec4 color);
	void Clear(float r, float g, float b, float a);

	Renderer();
	void DrawQuad();
	~Renderer();
};