#pragma once

#include <glad/glad.h>
#include <vector>
#include "../../Math/Precision.h"

class VBO {
public:
	uint32 ID;

	VBO(std::vector<float>* vertices);
	void Bind();
	void Unbind();
	void Delete();
	~VBO();
};