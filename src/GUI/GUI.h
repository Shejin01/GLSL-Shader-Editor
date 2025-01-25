#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Window/Window.h"
#include "TextEditor.h"
#include "../Window/Input.h"
#include "../Renderer/Shader.h"
#include "../Camera/Camera.h"
#include "../Texture/TextureManager.h"
#include "../Framebuffer/Framebuffer.h"
#include "../Renderer/Renderer.h"

struct GUIContext {
	TextEditor* editor;
	Input* input;
	Shader* shader;
	Camera* camera;
	String shaderFilepath = String(256, NULL);
	String vertexShaderCode;
	Renderer* renderer;
	Window* window;
};

class GUI {
private:
	struct FrameData {
		bool saveFile = false;
		bool saveAsFile = false;
		bool loadFile = false;
		bool compile = false;
		bool loadTexture = false;
	};
	struct Data {
		bool showPalettePicker = false;
		bool showColorPicker = false;
		bool showTextureBrowser = false;
		bool showCameraInfoOverlay = false;

		String selectedTexture = "";
		String textureFilepath = String(256, NULL);
		String textureName = String(256, NULL);
		String loadedShaderFilepath = "assets/Shaders/shader.fs";
		std::vector<String> recentFiles = { "assets/Shaders/shader.fs" };

		Shader paletteShader;

		float colorPalette[12] = {
			0.5, 0.5, 0.5,
			0.5, 0.5, 0.5,
			1.0, 1.0, 1.0,
			0.00, 0.33, 0.67
		};
		float color[4];
	};
	static Data data;
	static FrameData fdata;
	static FBO fbo;
	GUI() {}
	static void ShowColorPicker(GUIContext* context, bool* p_open);
	static void ShowPalettePicker(GUIContext* context, bool* p_open);
	static void ShowTextureBrowser(GUIContext* context, bool* p_open);
	static void ShowCameraInfoOverlay(GUIContext* context, bool* p_open);
 public:
	static void Init(Window* window);
	static void NewFrame();
	static void Process(GUIContext* context);
	static void Render();
	static void Shutdown();
};