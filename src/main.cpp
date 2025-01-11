#include "Window/Window.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "GUI/GUI.h"
#include "Buffers/VAO.h"
#include "Buffers/VBO.h"
#include "GUI/TextEditor.h"

const uint32 WIDTH = 800, HEIGHT = 600;
const String TITLE = "GLSL Shader Editor";

int main() {
	Window::Init();

	Window window(WIDTH, HEIGHT, TITLE);
	Input input(&window);
	GUI::Init(&window);

	Renderer renderer;
	String vertexShaderCode = LoadShaderFromFile("assets/Shaders/shader.vs");
	String fragmentShaderCode = LoadShaderFromFile("assets/Shaders/shader.fs");
	Shader shader;
	shader.Compile(vertexShaderCode.c_str(), fragmentShaderCode.c_str());

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

	float blendFactor = 0.05;
	while (!window.WindowShouldClose()) {
		input.PollEvents();
		if (input.GetKeyPressed(GLFW_KEY_ESCAPE)) window.SetWindowShouldClose(true);

		renderer.Clear(0.0, 0.0, 0.0, 1.0);
		GUI::NewFrame();

		shader.Use();

		shader.SetVec2("resolution", window.GetSize());
		shader.SetFloat("time", glfwGetTime());
		shader.SetVec2("cursorPos", input.GetCursorPos());

		shader.SetFloat("blendFactor", blendFactor);

		quadVao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		quadVao.Unbind();

		ImGui::Begin("Window", 0);
		ImGui::Text("Hello World");
		ImGui::DragFloat("Blend Factor", &blendFactor, 0.001);
		if (ImGui::Button("Compile Code") || input.GetKeyPressed(GLFW_KEY_F5)) {
			shader.Compile(vertexShaderCode, editor.GetText());
		}
		editor.Render("Code", ImVec2(), true);
		ImGui::End();

		GUI::Render();
		input.UpdateEvents();
		glfwSwapBuffers(window.ID);
	}

	shader.Delete();
	GUI::Shutdown();

	return 0;
}