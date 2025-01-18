#include "GUI.h"

GUI::Data GUI::data;

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
void GUI::Process(GUIContext* context) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	if (ImGui::Begin("Code Editor", 0, flags)) {
		FrameData fdata;
		context->editor->IsTextChanged();
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) context->editor->SetText(String());
				if (ImGui::MenuItem("Save", "Ctrl+S")) fdata.saveFile = true;
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) fdata.saveAsFile = true;
				if (ImGui::MenuItem("Open", "Ctrl+O")) fdata.loadFile = true;
				ImGui::Text("Press ESC to Quit");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) context->editor->Undo();
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) context->editor->Redo();
				if (ImGui::MenuItem("Cut", "Ctrl+X")) context->editor->Cut();
				if (ImGui::MenuItem("Copy", "Ctrl+C")) context->editor->Copy();
				if (ImGui::MenuItem("Paste", "Ctrl+V")) context->editor->Paste();
				ImGui::SeparatorText("Mouse");
				if (ImGui::MenuItem("Reset Camera Position")) {
					context->camera->position = glm::vec3(0.0f);
					context->camera->front = glm::vec3(0.0f, 0.0f, -1.0f);
					context->camera->yaw = -90.0f;
				};
				if (ImGui::MenuItem("Reset Scroll Amount")) context->input->SetScrollAmount(0.0);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Build")) {
				if (ImGui::MenuItem("Compile Code", "F5")) fdata.compile = true;
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools")) {
				ImGui::MenuItem("Color Picker", 0, &data.showColorPicker);
				ImGui::MenuItem("Palette Picker", 0, &data.showPalettePicker);
				ImGui::EndMenu();
			}
				
			if (ImGui::BeginMenu("Help")) {
				ImGui::Text("Press C while the UI is deselected to enable Camera Mode.");
				ImGui::Text("WASD - Camera Movement");
				ImGui::Text("Scroll Wheel - Change Camera Speed (Capped between 0.1 and 100.0)");
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		Input input = *context->input;
		bool isCtrlHeld = input.GetKeyHeld(GLFW_KEY_LEFT_CONTROL) || input.GetKeyHeld(GLFW_KEY_RIGHT_CONTROL);
		bool isShiftHeld = input.GetKeyHeld(GLFW_KEY_LEFT_SHIFT) || input.GetKeyHeld(GLFW_KEY_RIGHT_SHIFT);
		if (fdata.saveFile || (isCtrlHeld && !isShiftHeld && input.GetKeyPressed(GLFW_KEY_S))) {
			File::SaveToFile(context->filepath, context->editor->GetText());
			Logger::LogLine("Code Editor", "File Saved");
		}
		if (fdata.saveAsFile || (isCtrlHeld && isShiftHeld && input.GetKeyPressed(GLFW_KEY_S))) ImGui::OpenPopup("Save As File");
		if (fdata.loadFile || (isCtrlHeld && input.GetKeyPressed(GLFW_KEY_O))) ImGui::OpenPopup("Open File");
		if (fdata.compile || input.GetKeyPressed(GLFW_KEY_F5)) context->shader->Compile(context->vertexShaderCode, context->editor->GetText());

		if (ImGui::BeginPopupModal("Save As File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &context->filepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Save")) {
				File::SaveToFile(context->filepath, context->editor->GetText());
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Open File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &context->filepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Load")) {
				context->editor->SetText(File::LoadFromFile(context->filepath));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (data.showColorPicker) ShowColorPicker(context, &data.showColorPicker);
		if (data.showPalettePicker) ShowPalettePicker(context, &data.showPalettePicker);

		ImGui::Text("FPS: %.3f", ImGui::GetIO().Framerate);
		ImGui::Text("Total Lines: %i, Line: %i | Col: %i", context->editor->GetTotalLines(), context->editor->GetCursorPosition().mLine, context->editor->GetCursorPosition().mColumn);
		context->editor->Render("Code", ImVec2(), true);	
	}
	ImGui::End();
}
void GUI::ShowColorPicker(GUIContext* context, bool* p_open) {
	if (ImGui::Begin("Color Picker", p_open)) {
		ImGui::ColorPicker4("Color", data.color);
	}
	ImGui::End();
}
void GUI::ShowPalettePicker(GUIContext* context, bool* p_open) {
	if (ImGui::Begin("Palette Picker", p_open)) {		
		ImGui::ColorEdit3("Color 1", &data.colorPalette[0]);
		ImGui::ColorEdit3("Color 2", &data.colorPalette[3]);
		ImGui::ColorEdit3("Color 3", &data.colorPalette[6]);
		ImGui::ColorEdit3("Color 4", &data.colorPalette[9]);
	}
	ImGui::End();
}

void GUI::Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void GUI::Shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}