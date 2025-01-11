#include "GUI.h"

void GUI::Init(Window* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->ID, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}
void GUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void GUI::Render(GUIContext& context) {
	if (ImGui::Begin("Code Editor", 0, ImGuiWindowFlags_MenuBar)) {
		Data data;
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) context.editor->SetText(String());
				if (ImGui::MenuItem("Save", "Ctrl+S")) data.saveFile = true;
				if (ImGui::MenuItem("Open", "Ctrl+O")) data.loadFile = true;
				ImGui::Text("Press ESC to Quit");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) context.editor->Undo();
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) context.editor->Redo();
				if (ImGui::MenuItem("Cut", "Ctrl+X")) context.editor->Cut();
				if (ImGui::MenuItem("Copy", "Ctrl+C")) context.editor->Copy();
				if (ImGui::MenuItem("Paste", "Ctrl+V")) context.editor->Paste();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Build")) {
				if (ImGui::MenuItem("Compile Code", "F5")) data.compile = true;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (data.saveFile || (context.input->GetKeyHeld(GLFW_KEY_LEFT_CONTROL) && context.input->GetKeyPressed(GLFW_KEY_S))) ImGui::OpenPopup("Save File");
		if (data.loadFile || (context.input->GetKeyHeld(GLFW_KEY_LEFT_CONTROL) && context.input->GetKeyPressed(GLFW_KEY_O))) ImGui::OpenPopup("Open File");
		if (data.compile || context.input->GetKeyPressed(GLFW_KEY_F5)) context.shader->Compile(context.vertexShaderCode, context.editor->GetText());

		if (ImGui::BeginPopupModal("Save File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &context.filepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Save")) {
				File::SaveToFile(context.filepath, context.editor->GetText());
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Open File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &context.filepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Load")) {
				context.editor->SetText(File::LoadFromFile(context.filepath));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		ImGui::Text("Total Lines: %i, Line: %i | Col: %i", context.editor->GetTotalLines(), context.editor->GetCursorPosition().mLine, context.editor->GetCursorPosition().mColumn);

		context.editor->Render("Code", ImVec2(), true);
		
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void GUI::Shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}