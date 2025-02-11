#pragma once

#include "Camera/Camera.h"
#include "Config.h"
#include "File Loader/FileLoader.h"
#include "GUI/GUI.h"
#include "GUI/TextEditor.h"
#include "Logger/Logger.h"
#include "Renderer/Buffers/VAO.h"
#include "Renderer/Buffers/VBO.h"
#include "Renderer/Renderer.h"
#include "Texture/Texture.h"
#include "Texture/CubemapManager.h"
#include "Texture/TextureManager.h"
#include "Window/Input.h"
#include "Window/Window.h"

class Application {
public:
	Application(Config& config);
	void Run();
private:
	Camera camera;
	Config config;
	GUIContext context;
	TextEditor editor;
	Input input;
	Renderer renderer;
	Shader shader;
	Window window;
	Cubemap cubemap;

	void Init();
	void Update();
	void Render();
	void Shutdown();
};