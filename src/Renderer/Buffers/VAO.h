#pragma once

#include <glad/glad.h>
#include "../../Math/Precision.h"

#include <stdlib.h>

class VAO {
public:
	uint32 ID;

	VAO();
	void LinkAttrib(uint32 location, uint32 size, uint32 stride, uint32 offset);
	void Bind();
	void Unbind();
	void Delete();
	~VAO();
};