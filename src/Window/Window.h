#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Math/Precision.h"
#include "../Logger/Logger.h"
#include <vector>
#include <glm/glm.hpp>

class Window {
private:
	uint32 width, height;
	String title;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static std::vector<Window*> instances;
public:
	GLFWwindow* ID;
	
	static void Init();
	Window(uint32 width, uint32 height, String title);
	
	bool WindowShouldClose() const;
	void SetWindowShouldClose(bool value);

	void Maximize();
	void Minimize();

	void SetSize(uint32 width, uint32 height);
	glm::vec2 GetSize() const;
	String GetTitle() const;
	void SetTitle(String title);

	void Delete();
	~Window();
};