#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Window/Window.h"

class GUI {
private:
	GUI() {}
public:
	static void Init(Window* window);
	static void NewFrame();
	static void Render();
	static void Shutdown();
};