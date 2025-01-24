#include "GUI.h"

GUI::Data GUI::data;
GUI::FrameData GUI::fdata;
FBO GUI::fbo;

void GUI::Init(Window* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->ID, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	String vertexShaderCode = File::LoadFromFile("assets/Shaders/shader.vs");
	String fPaletteShaderCode =
		"#version 330 core\n"
		"in vec2 TexCoord;"
		"uniform vec3 a;\n"
		"uniform vec3 b;\n"
		"uniform vec3 c;\n"
		"uniform vec3 d;\n"
		"void main() {\n"
		"	float t = TexCoord.x;\n"
		"	gl_FragColor = vec4(a + b * cos(6.283185 * (c * t + d)), 1.0f);\n"
		"}\n";
	data.paletteShader.Compile(vertexShaderCode.c_str(), fPaletteShaderCode.c_str());
}
void GUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void GUI::Process(GUIContext* context) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	if (ImGui::Begin("Code Editor", 0, flags)) {
		fdata = FrameData();
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
				ImGui::MenuItem("Texture Browser", 0, &data.showTextureBrowser);
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
			File::SaveToFile(context->shaderFilepath, context->editor->GetText());
			Logger::LogLine("Code Editor", "File Saved");
		}
		if (fdata.saveAsFile || (isCtrlHeld && isShiftHeld && input.GetKeyPressed(GLFW_KEY_S))) ImGui::OpenPopup("Save As File");
		if (fdata.loadFile || (isCtrlHeld && input.GetKeyPressed(GLFW_KEY_O))) ImGui::OpenPopup("Open File");
		if (fdata.compile || input.GetKeyPressed(GLFW_KEY_F5)) context->shader->Compile(context->vertexShaderCode, context->editor->GetText());

		if (ImGui::BeginPopupModal("Save As File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &context->shaderFilepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Save")) {
				File::SaveToFile(context->shaderFilepath, context->editor->GetText());
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Open File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("File Path", &context->shaderFilepath[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Load")) {
				context->editor->SetText(File::LoadFromFile(context->shaderFilepath));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		ImGui::Text("FPS: %.3f (%.2fms)", ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime*1000.0f);
		ImGui::Text("Total Lines: %i, Line: %i | Col: %i", context->editor->GetTotalLines(), context->editor->GetCursorPosition().mLine, context->editor->GetCursorPosition().mColumn);
		context->editor->Render("Code", ImVec2(), true);	
	}
	ImGui::End();

	if (data.showColorPicker) ShowColorPicker(context, &data.showColorPicker);
	if (data.showPalettePicker) ShowPalettePicker(context, &data.showPalettePicker);
	if (data.showTextureBrowser) ShowTextureBrowser(context, &data.showTextureBrowser);
}

void GUI::ShowColorPicker(GUIContext* context, bool* p_open) {
	if (ImGui::Begin("Color Picker", p_open)) {
		ImGui::ColorPicker4("Color", data.color);
		if (ImGui::Button("Copy Color As GLSL Code")) {
			String string = "vec3 color = vec3(" + std::to_string(data.color[0] ) + ", " + std::to_string(data.color[1]) + ", " + std::to_string(data.color[2]) + ");";
			ImGui::SetClipboardText(string.c_str());
		}
	}
	ImGui::End();
}
void GUI::ShowPalettePicker(GUIContext* context, bool* p_open) {
	if (ImGui::Begin("Palette Picker", p_open)) {
		fbo = FBO(ImGui::GetWindowWidth()-15, 1);
		fbo.Bind();
		data.paletteShader.Use();
		data.paletteShader.SetMat4("model", glm::scale(glm::mat4(1.0), glm::vec3(1.0)));
		data.paletteShader.SetVec3("a", data.colorPalette[0], data.colorPalette[1], data.colorPalette[2]);
		data.paletteShader.SetVec3("b", data.colorPalette[3], data.colorPalette[4], data.colorPalette[5]);
		data.paletteShader.SetVec3("c", data.colorPalette[6], data.colorPalette[7], data.colorPalette[8]);
		data.paletteShader.SetVec3("d", data.colorPalette[9], data.colorPalette[10], data.colorPalette[11]);
		context->renderer->DrawQuad();
		fbo.Unbind(context->window->GetSize());

		ImGui::Image(fbo.GetTextureID(), ImVec2(ImGui::GetWindowWidth() - 15, 100));
		ImGui::DragFloat3("Brightness", &data.colorPalette[0], 0.01, -5.0f, 5.0f);
		ImGui::DragFloat3("Contrast", &data.colorPalette[3], 0.01, -5.0f, 5.0f);
		ImGui::DragFloat3("Frequency", &data.colorPalette[6], 0.01, -5.0f, 5.0f);
		ImGui::DragFloat3("Phase Offset", &data.colorPalette[9], 0.01, -5.0f, 5.0f);
		if (ImGui::Button("Copy Palette As GLSL Code")) {
			String string =
				"vec3 color1 = vec3(" + std::to_string(data.colorPalette[0]) + ", " + std::to_string(data.colorPalette[1]) + ", " + std::to_string(data.colorPalette[2]) + ");\n"
				"vec3 color2 = vec3(" + std::to_string(data.colorPalette[3]) + ", " + std::to_string(data.colorPalette[4]) + ", " + std::to_string(data.colorPalette[5]) + ");\n"
				"vec3 color3 = vec3(" + std::to_string(data.colorPalette[6]) + ", " + std::to_string(data.colorPalette[7]) + ", " + std::to_string(data.colorPalette[8]) + ");\n"
				"vec3 color4 = vec3(" + std::to_string(data.colorPalette[9]) + ", " + std::to_string(data.colorPalette[10]) + ", " + std::to_string(data.colorPalette[11]) + ");";
			ImGui::SetClipboardText(string.c_str());
		}
		ImGui::Text("Courtesy of Inigo Quilez:");
		ImGui::TextLinkOpenURL("https://iquilezles.org/articles/palettes/");
	}
	ImGui::End();
}
void GUI::ShowTextureBrowser(GUIContext* context, bool* p_open) {
	if (ImGui::Begin("Texture Browser", p_open, ImGuiWindowFlags_MenuBar)) {
		// Menu Bar
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Load Texture")) fdata.loadTexture = true;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (fdata.loadTexture) ImGui::OpenPopup("Load Texture");

		// Load Texture Popup
		if (ImGui::BeginPopupModal("Load Texture", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("Texture File Path", &data.textureFilepath[0], 256);
			ImGui::InputText("Texture Name", &data.textureName[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Load")) {
				TextureManager::AddTexture(data.textureName, data.textureFilepath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		// Textures Section
		float windowHalfWidth = (ImGui::GetWindowWidth() - 25) * 0.5;
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("Textures", ImVec2(windowHalfWidth, 0), ImGuiChildFlags_Borders);
		int i = 0;
		for (auto iter : TextureManager::textures) {
			if (ImGui::ImageButton(iter.first.c_str(), iter.second.ID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0))) {
				data.selectedTexture = iter.first;
			}
			ImGui::SameLine();
			i++;
			if (i % (int)(std::fmax((windowHalfWidth / 116), 1)) == 0) ImGui::NewLine();
		}
		ImGui::EndChild();
		ImGui::SameLine();

		// Properties Section
		ImGui::BeginChild("Properties", ImVec2(windowHalfWidth, 0), ImGuiChildFlags_Borders);
		if (data.selectedTexture != "") {
			Texture* texture = TextureManager::GetTexture(data.selectedTexture);
			ImGui::Image(texture->ID, ImVec2(windowHalfWidth-15, windowHalfWidth-15), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::Text(("Selected Texture: " + data.selectedTexture).c_str());
			ImGui::Text(("Texture Path: " + texture->GetTexturePath()).c_str());
			ImGui::Text("Resolution: %ix%i", texture->GetWidth(), texture->GetHeight());
			ImGui::Text("Number of Color Channels: %i", texture->GetNumberOfChannels());
			if (ImGui::Button("Delete")) {
				TextureManager::DeleteTexture(data.selectedTexture);
				data.selectedTexture = "";
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
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