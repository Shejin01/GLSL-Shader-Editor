#include "Camera/Camera.h"
#include "File Loader/FileLoader.h"
#include "GUI/GUI.h"
#include "GUI/TextEditor.h"
#include "Logger/Logger.h"
#include "Renderer/Buffers/VAO.h"
#include "Renderer/Buffers/VBO.h"
#include "Renderer/Renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Texture/Texture.h"
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
	context.camera = &camera;
	context.filepath = String(256, NULL);
	context.vertexShaderCode = vertexShaderCode;

	Texture texture("assets/Textures/container_diffuse.png");

	shader.Use();
	shader.SetInt("ourTexture", 0);

	uint32 iFrame = 0;
	glm::vec4 iMouse;
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
		shader.SetVec2("iResolution", window.GetSize());
		shader.SetFloat("iTime", glfwGetTime());
		shader.SetFloat("iTimeDelta", dt);
		shader.SetFloat("iFrameRate", ImGui::GetIO().Framerate);
		shader.SetInt("iFrame", iFrame);
		shader.SetVec4("iMouse", iMouse);
		shader.SetVec2("iCursorPos", iCursorPos);
		shader.SetMat4("iViewMatrix", camera.GetViewMatrix());
		shader.SetVec3("iCameraPos", camera.position);
		shader.SetFloat("iScrollOffset", input.GetScrollOffset());
		shader.SetFloat("iScrollAmount", input.GetScrollAmount());

		texture.Bind(0);

		quadVao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		quadVao.Unbind();

		GUI::Process(&context);

		ImGui::ShowDemoWindow();

		GUI::Render();
		glfwSwapBuffers(window.ID);
		input.UpdateEvents();
		iFrame++;
	}

	GUI::Shutdown();
	texture.Delete();

	return 0;
}