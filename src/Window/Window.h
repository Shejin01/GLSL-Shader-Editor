#pragma once

#include "../Math/Precision.h"
#include "../Logger/Logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class Window {
private:
	uint32 width, height;
	String title;
	static std::vector<Window*> instances;
	GLFWmonitor* monitor;
	const GLFWvidmode* mode;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
public:
	GLFWwindow* ID;
	
	static void Init();
	Window();
	bool CreateWindow(uint32 width, uint32 height, String title);
	void SwapBuffers();

	bool WindowShouldClose() const;
	void SetWindowShouldClose(bool value);

	void BorderlessFullscreen();
	void WindowedFullscreen();
	void Minimize();

	void SetSize(uint32 width, uint32 height);
	glm::vec2 GetSize() const;
	String GetTitle() const;
	void SetTitle(String title);

	void Delete();
};