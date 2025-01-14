#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Window/Window.h"
#include "TextEditor.h"
#include "../Window/Input.h"
#include "../Renderer/Shader.h"

struct GUIContext {
	TextEditor* editor;
	Input* input;
	Shader* shader;
	String filepath;
	String vertexShaderCode;
};

class GUI {
private:
	struct Data {
		bool saveFile = false;
		bool saveAsFile = false;
		bool loadFile = false;
		bool compile = false;
	};

	GUI() {}
public:
	static void Init(Window* window);
	static void NewFrame();
	static void Process(GUIContext& context);
	static void Render();
	static void Shutdown();
};