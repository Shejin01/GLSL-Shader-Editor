#pragma once

#include <glad/glad.h>
#include "Buffers/VAO.h"
#include "Buffers/VBO.h"
#include "Shader.h"

class Renderer {
private:
	VBO quadVbo;
	VAO quadVao;
public:
	void Clear(glm::vec4 color);
	void Clear(float r, float g, float b, float a);

	void Init();
	void DrawQuad();
	void Delete();
};