#include "Buffers/VAO.h"
#include "Buffers/VBO.h"
#include "File Loader/FileLoader.h"
#include "GUI/GUI.h"
#include "GUI/TextEditor.h"
#include "Input/Input.h"
#include "Logger/Logger.h"
#include "Renderer/Renderer.h"
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

	String filepath(256, NULL);
	while (!window.WindowShouldClose()) {
		input.PollEvents();
		if (input.GetKeyPressed(GLFW_KEY_ESCAPE)) window.SetWindowShouldClose(true);

		renderer.Clear(0.0, 0.0, 0.0, 1.0);
		GUI::NewFrame();

		shader.Use();

		shader.SetVec2("resolution", window.GetSize());
		shader.SetFloat("time", glfwGetTime());
		shader.SetVec2("cursorPos", input.GetCursorPos());

		quadVao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		quadVao.Unbind();

		ImGui::Begin("Code Editor");
		if (ImGui::Button("Compile Code") || input.GetKeyPressed(GLFW_KEY_F5)) {
			shader.Compile(vertexShaderCode, editor.GetText());
		} ImGui::SameLine();
		if (ImGui::Button("Save")) ImGui::OpenPopup("Save File"); ImGui::SameLine();
		if (ImGui::Button("Load")) ImGui::OpenPopup("Load File"); ImGui::SameLine(0, 50.0f);
		if (ImGui::Button("Undo")) editor.Undo(); ImGui::SameLine();
		if (ImGui::Button("Redo")) editor.Redo(); ImGui::SameLine();
		if (ImGui::Button("Cut")) editor.Cut(); ImGui::SameLine();
		if (ImGui::Button("Copy")) editor.Copy(); ImGui::SameLine();
		if (ImGui::Button("Paste")) editor.Paste(); ImGui::SameLine();
		if (ImGui::Button("Delete")) editor.Delete();

		if (ImGui::BeginPopupModal("Save File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &filepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Save")) {
				File::SaveToFile(filepath, editor.GetText());
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Load File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &filepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Load")) {
				editor.SetText(File::LoadFromFile(filepath));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		ImGui::Text("Total Lines: %i, Line: %i, Col: %i", editor.GetTotalLines(), editor.GetCursorPosition().mLine, editor.GetCursorPosition().mColumn);

		editor.Render("Code", ImVec2(), true);
		ImGui::End();

		ImGui::ShowDemoWindow();

		GUI::Render();
		input.UpdateEvents();
		glfwSwapBuffers(window.ID);
	}

	shader.Delete();
	GUI::Shutdown();

	return 0;
}