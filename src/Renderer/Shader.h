#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../File Loader/FileLoader.h"
#include "../Logger/Logger.h"
#include "../Math/Precision.h"

class Shader {
private:
	void CheckError(uint32 object, String type);
public:
	uint32 ID;

	void Compile(String vShaderCode, String fShaderCode);
	void Use();

	void SetBool(String name, bool value);
	void SetInt(String name, int value);
	void SetFloat(String name, float value);
	void SetVec2(String name, glm::vec2 value);
	void SetVec2(String name, float v0, float v1);
	void SetVec3(String name, glm::vec3 value);
	void SetVec3(String name, float v0, float v1, float v2);
	void SetVec4(String name, glm::vec4 value);
	void SetVec4(String name, float v0, float v1, float v2, float v3);
	void SetMat4(String name, glm::mat4 value);

	void Delete();
};