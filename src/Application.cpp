#include "Application.h"

Application::Application(Config& config) : config(config) {}
void Application::Run() {
	Init();
	Update();
	Shutdown();
}

void Application::Init() {
	Window::Init();

	// TODO: Exit Popup only appears when Code Editor Window is not hidden

	// Create Window and Input Manager
	window.CreateWindow(config.WIDTH, config.HEIGHT, config.TITLE);
	window.WindowedFullscreen();
	input.CreateInstance(&window);
	GUI::Init(&window);

	// Initiate Renderer and Main Shader
	renderer.Init();
	String vertexShaderCode = File::LoadFromFile("assets/Shaders/shader.vs");
	String fragmentShaderCode = File::LoadFromFile("assets/Shaders/shader.fs");
	shader.Compile(vertexShaderCode.c_str(), fragmentShaderCode.c_str());

	// Configure Text Editor
	editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
	editor.SetText(fragmentShaderCode);
	editor.SetShowWhitespaces(false);

	// Load Example Textures
	std::vector<String> texturePaths = {
		"assets/Cubemaps/right.jpg",
		"assets/Cubemaps/left.jpg",
		"assets/Cubemaps/top.jpg",
		"assets/Cubemaps/bottom.jpg",
		"assets/Cubemaps/front.jpg",
		"assets/Cubemaps/back.jpg"
	};
	std::vector<String> texturePaths2 = {
		"assets/Cubemaps/milkyway_right.png",
		"assets/Cubemaps/milkyway_left.png",
		"assets/Cubemaps/milkyway_top.png",
		"assets/Cubemaps/milkyway_bottom.png",
		"assets/Cubemaps/milkyway_front.png",
		"assets/Cubemaps/milkyway_back.png"
	};
	cubemap = Cubemap(texturePaths);
	CubemapManager::AddCubemap("cubemap0", &cubemap);
	CubemapManager::AddCubemap("milkyway", texturePaths2);

	//stbi_set_flip_vertically_on_load(true);
	Texture texture("assets/Textures/container_diffuse.png");
	TextureManager::AddTexture("texture0", &texture);
	TextureManager::AddTexture("texture1", "assets/Textures/pepe.jpg");
	TextureManager::AddTexture("texture2", "assets/Textures/smiley.png");
	TextureManager::AddTexture("texture3", "assets/Textures/wojak.png");

	// Set GUIContext
	context.editor = &editor;
	context.shader = &shader;
	context.input = &input;
	context.camera = &camera;
	context.vertexShaderCode = vertexShaderCode;
	context.renderer = &renderer;
	context.window = &window;
}

void Application::Update() {
	uint32 iFrame = 0;
	glm::vec4 iMouse;
	float prevTime = 0.0f;

	while (!window.WindowShouldClose()) {
		// Calculate Delta Time
		float crntTime = glfwGetTime();
		float dt = crntTime - prevTime;
		prevTime = crntTime;

		// Process Events
		input.PollEvents();
		camera.ProcessInputs(&input, dt);

		// Shadertoy iMouse Logic
		glm::vec2 iCursorPos = input.GetCursorPos();
		iCursorPos.y = window.GetSize().y - iCursorPos.y;
		if (input.GetMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT) || input.GetMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) {
			iMouse.x = iCursorPos.x;
			iMouse.y = iCursorPos.y;
		}
		if (input.GetMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) || input.GetMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
			iMouse.z = iMouse.x;
			iMouse.w = iMouse.y;
		}
		else if (input.GetMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT) || input.GetMouseButtonReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
			iMouse.z *= -1;
			iMouse.w *= -1;
		}

		// Set Shader Uniforms
		shader.Use();
		shader.SetMat4("model", glm::mat4(1.0));
		shader.SetVec2("iResolution", window.GetSize());
		shader.SetFloat("iTime", glfwGetTime());
		shader.SetFloat("iTimeDelta", dt);
		shader.SetFloat("iFrameRate", ImGui::GetIO().Framerate);
		shader.SetInt("iFrame", iFrame);
		shader.SetVec4("iMouse", iMouse);
		shader.SetVec2("mouse.iCursorPos", iCursorPos);
		shader.SetFloat("mouse.iScrollOffset", input.GetScrollOffset());
		shader.SetFloat("mouse.iScrollAmount", input.GetScrollAmount());
		shader.SetMat4("camera.iViewMatrix", camera.GetViewMatrix());
		shader.SetVec3("camera.iCameraPos", camera.position);
		shader.SetVec3("camera.iCameraDir", camera.front);

		Render();
		input.UpdateEvents();
		iFrame++;
	}
}

void Application::Render() {
	renderer.Clear(0.0, 0.0, 0.0, 1.0);
	GUI::NewFrame();
	TextureManager::BindAllTextures(&shader);
	CubemapManager::BindAllCubemaps(&shader);
	renderer.DrawQuad();

	GUI::Process(&context);
	GUI::Render();
	window.SwapBuffers();
}

void Application::Shutdown() {
	GUI::Shutdown();
	TextureManager::DeleteAllTextures();
	CubemapManager::DeleteAllCubemaps();
	window.Delete();
	shader.Delete();
	renderer.Delete();
}