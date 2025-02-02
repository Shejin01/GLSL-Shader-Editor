#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "TextEditor.h"
#include "../Camera/Camera.h"
#include "../Framebuffer/Framebuffer.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Shader.h"
#include "../Texture/TextureManager.h"
#include "../Window/Input.h"
#include "../Window/Window.h"

struct GUIContext {
	Camera*		camera;
	TextEditor*	editor;
	Input*		input;
	Renderer*	renderer;
	Shader*		shader;
	String		shaderFilepath = String(256, NULL);
	String		vertexShaderCode;
	Window*		window;
};

class GUI {
private:
	struct FrameData {
		bool compile = false;
		bool loadFile = false;
		bool loadTexture = false;
		bool saveFile = false;
		bool saveAsFile = false;
		bool exit = false;
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
			0.50, 0.50, 0.50,
			0.50, 0.50, 0.50,
			1.00, 1.00, 1.00,
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

	static void OpenSaveAsPopup(GUIContext* context);
	static void OpenLoadFilePopup(GUIContext* context);
	static void OpenExitPopup(GUIContext* context);
 public:
	static void Init(Window* window);
	static void NewFrame();
	static void Process(GUIContext* context);
	static void Render();
	static void Shutdown();
};