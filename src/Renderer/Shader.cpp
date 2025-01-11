#include "Shader.h"

void Shader::Compile(String vShaderCode, String fShaderCode) {
	const char* vShaderSource = vShaderCode.c_str();
	const char* fShaderSource = fShaderCode.c_str();
	uint32 vertex, fragment;
	
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderSource, nullptr);
	glCompileShader(vertex);
	CheckError(vertex, "Vertex");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderSource, nullptr);
	glCompileShader(fragment);
	CheckError(fragment, "Fragment");

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	CheckError(ID, "Program");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
void Shader::Use() {
	glUseProgram(ID);
}
void Shader::CheckError(uint32 object, String type) {
	int32 success;
	const uint32 infoLogSize = 1024;
	char infoLog[infoLogSize];
	if (type == "Program") {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, infoLogSize, nullptr, infoLog);
			Logger::Log("Shader", std::string("Failed to link program.\n") + infoLog);
		}
	}
	else if (type == "Vertex" || type == "Fragment") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, infoLogSize, nullptr, infoLog);
			Logger::Log("Shader", std::string("Failed to compile ") + type + std::string(" shader.\n") + infoLog);
		}
	}
}

void Shader::SetBool(String name, bool value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetInt(String name, int value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetFloat(String name, float value) {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetVec2(String name, glm::vec2 value) {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}
void Shader::SetVec2(String name, float v0, float v1) {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), v0, v1);
}
void Shader::SetVec3(String name, glm::vec3 value) {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}
void Shader::SetVec3(String name, float v0, float v1, float v2) {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2);
}
void Shader::SetVec4(String name, glm::vec4 value) {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}
void Shader::SetVec4(String name, float v0, float v1, float v2, float v3) {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2, v3);
}
void Shader::SetMat4(String name, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, false, glm::value_ptr(value));
}

void Shader::Delete() {
	glDeleteProgram(ID);
}

String LoadShaderFromFile(String shaderPath) {
	String shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		std::stringstream shaderStream;
		shaderFile.open(shaderPath.c_str());
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e) {
		Logger::LogLine("Shader", std::string("Failed to load shader from: ") + shaderPath + '\n' + e.what());
	}

	return shaderCode;
}