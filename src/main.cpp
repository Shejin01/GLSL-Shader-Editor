#include "Camera/Camera.h"
#include "File Loader/FileLoader.h"
#include "GUI/GUI.h"
#include "GUI/TextEditor.h"
#include "Logger/Logger.h"
#include "Renderer/Buffers/VAO.h"
#include "Renderer/Buffers/VBO.h"
#include "Renderer/Renderer.h"
#include "Window/Input.h"
#include "Window/Window.h"

const uint32 WIDTH = 800, HEIGHT = 600;
const String TITLE = "GLSL Shader Editor";

int main() {
	Window::Init();

	Window window(WIDTH, HEIGHT, TITLE);
	window.Maximize();
	Input input(&window);
	GUI::Init(&window);

	Renderer renderer;
	String vertexShaderCode = File::LoadFromFile("assets/Shaders/shader.vs");
	String fragmentShaderCode = File::LoadFromFile("assets/Shaders/shader.fs");
	Shader shader;
	shader.Compile(vertexShaderCode.c_str(), fragmentShaderCode.c_str());

	Camera camera(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	std::vector<float> vertices = {
		-1.0f, -1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 0.0f,
		 1.0f,  1.0f,	1.0f, 1.0f,

		 1.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,	0.0f, 0.0f
	};

	VAO quadVao;
	quadVao.Bind();
	VBO quadVbo(&vertices);
	quadVbo.Bind();
	quadVao.LinkAttrib(0, 2, 4 * sizeof(float), 0);
	quadVao.LinkAttrib(1, 2, 4 * sizeof(float), 2 * sizeof(float));
	quadVao.Unbind();
	quadVbo.Unbind();

	TextEditor editor;
	editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
	editor.SetText(fragmentShaderCode);
	editor.SetShowWhitespaces(false);

	GUIContext context;
	context.editor = &editor;
	context.shader = &shader;
	context.input = &input;
	context.filepath = String(256, NULL);
	context.vertexShaderCode = vertexShaderCode;

	float prevTime = 0.0f;
	while (!window.WindowShouldClose()) {
		float crntTime = glfwGetTime();
		float dt = crntTime - prevTime;
		prevTime = crntTime;

		input.PollEvents();
		if (input.GetKeyPressed(GLFW_KEY_ESCAPE)) window.SetWindowShouldClose(true);
		camera.ProcessInputs(&input, dt);

		renderer.Clear(0.0, 0.0, 0.0, 1.0);
		GUI::NewFrame();

		shader.Use();

		shader.SetVec2("resolution", window.GetSize());
		shader.SetFloat("time", glfwGetTime());
		shader.SetVec2("cursorPos", input.GetCursorPos());
		shader.SetMat4("viewMatrix", camera.GetViewMatrix());

		quadVao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		quadVao.Unbind();

		GUI::Process(context);

		GUI::Render();
		glfwSwapBuffers(window.ID);
		input.UpdateEvents();
	}

	GUI::Shutdown();

	return 0;
}