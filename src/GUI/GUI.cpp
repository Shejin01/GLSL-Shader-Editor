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

	std::vector<String> recentFiles = File::LoadLinesFromFile("recent.txt");
	if (recentFiles.size() != 0) data.recentFiles = recentFiles;
}
void GUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void GUI::Process(GUIContext* context) {
	if (context->input->GetKeyPressed(GLFW_KEY_F3)) data.showGUI = !data.showGUI;
	if (!data.showGUI) return;
	if (ImGui::Begin("Code Editor", 0, ImGuiWindowFlags_MenuBar)) {
		fdata = FrameData();
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) {
					context->editor->SetText(String());
					data.loadedShaderFilepath = "";
				}
				if (ImGui::MenuItem("Save", "Ctrl+S")) fdata.saveFile = true;
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) fdata.saveAsFile = true;
				if (ImGui::MenuItem("Open", "Ctrl+O")) fdata.loadFile = true;
				if (ImGui::BeginMenu("Recent Files")) {
					for (int i = data.recentFiles.size()-1; i >= 0; i--) {
						if (ImGui::MenuItem(data.recentFiles[i].c_str())) {
							context->editor->SetText(File::LoadFromFile(data.recentFiles[i]));
							data.loadedShaderFilepath = data.recentFiles[i];
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Exit", "Escape")) fdata.exit = true;
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

			if (ImGui::BeginMenu("View")) {
				if (ImGui::MenuItem("Borderless Fullscreen")) context->window->BorderlessFullscreen();
				if (ImGui::MenuItem("Windowed Fullscreen")) context->window->WindowedFullscreen();
				if (ImGui::MenuItem("Show GUI", 0, &data.showGUI));
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
				ImGui::MenuItem("Camera Info", 0, &data.showCameraInfoOverlay);
				ImGui::EndMenu();
			}
				
			if (ImGui::BeginMenu("Help")) {
				ImGui::Text("Press F2 to enable Camera Mode.");
				ImGui::Text("WASD - Camera Movement");
				ImGui::Text("Scroll Wheel - Change Camera Speed (Capped between 0.1 and 100.0)");
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		Input input = *context->input;
		bool ctrl = input.GetKeyHeld(GLFW_KEY_LEFT_CONTROL) || input.GetKeyHeld(GLFW_KEY_RIGHT_CONTROL);
		bool shift = input.GetKeyHeld(GLFW_KEY_LEFT_SHIFT) || input.GetKeyHeld(GLFW_KEY_RIGHT_SHIFT);
		if (ctrl) {
			if (input.GetKeyPressed(GLFW_KEY_S)) {
				if (shift) fdata.saveAsFile = true;
				else fdata.saveFile = true;
			}
			if (input.GetKeyPressed(GLFW_KEY_O)) fdata.loadFile = true;
		}
		if (input.GetKeyPressed(GLFW_KEY_F5)) fdata.compile = true;
		if (input.GetKeyPressed(GLFW_KEY_ESCAPE)) fdata.exit = true;

		if (fdata.saveFile) {
			File::SaveToFile(data.loadedShaderFilepath, context->editor->GetText());
			context->editor->GetText();
			Logger::LogLine("Code Editor", "File Saved");
		}
		if (fdata.saveAsFile) ImGui::OpenPopup("Save As File");
		if (fdata.loadFile) ImGui::OpenPopup("Open File");
		if (fdata.compile) context->shader->Compile(context->vertexShaderCode, context->editor->GetText());
		if (fdata.exit) ImGui::OpenPopup("Exit Popup");

		OpenSaveAsPopup(context);
		OpenLoadFilePopup(context);
		OpenExitPopup(context);

		ImGui::Text("FPS: %.3f (%.2fms)", ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime*1000.0f);
		ImGui::Text((String("Filepath: ") + data.loadedShaderFilepath).c_str());
		ImGui::Text("Total Lines: %i, Line: %i | Col: %i", context->editor->GetTotalLines(), context->editor->GetCursorPosition().mLine, context->editor->GetCursorPosition().mColumn);
		context->editor->Render("Code", ImVec2(), true);	
	}
	ImGui::End();

	if (data.showColorPicker) ShowColorPicker(context, &data.showColorPicker);
	if (data.showPalettePicker) ShowPalettePicker(context, &data.showPalettePicker);
	if (data.showTextureBrowser) ShowTextureBrowser(context, &data.showTextureBrowser);
	if (data.showCameraInfoOverlay) ShowCameraInfoOverlay(context, &data.showCameraInfoOverlay);
}

void GUI::ShowColorPicker(GUIContext* context, bool* p_open) {
	if (ImGui::Begin("Color Picker", p_open)) {
		ImGui::ColorPicker4("Color", data.color);
		if (ImGui::Button("Copy Color As GLSL Code")) {
			String string = "vec3(" + std::to_string(data.color[0] ) + ", " + std::to_string(data.color[1]) + ", " + std::to_string(data.color[2]) + ");";
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
				if (ImGui::MenuItem("Load Cubemap")) fdata.loadCubemap = true;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (fdata.loadTexture) ImGui::OpenPopup("Load Texture");
		if (fdata.loadCubemap) ImGui::OpenPopup("Load Cubemap");

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
		// Load Cubemap Popup
		if (ImGui::BeginPopupModal("Load Cubemap", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("Right Texture Path", &data.cubemapFilepaths[0][0], 256);
			ImGui::InputText("Left Texture Path", &data.cubemapFilepaths[1][0], 256);
			ImGui::InputText("Top Texture Path", &data.cubemapFilepaths[2][0], 256);
			ImGui::InputText("Bottom Texture Path", &data.cubemapFilepaths[3][0], 256);
			ImGui::InputText("Front Texture Path", &data.cubemapFilepaths[4][0], 256);
			ImGui::InputText("Back Texture Path", &data.cubemapFilepaths[5][0], 256);

			ImGui::InputText("Cubemap Name", &data.cubemapName[0], 256);
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Load")) {
				CubemapManager::AddCubemap(data.cubemapName, data.cubemapFilepaths);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		// Textures Section
		float windowHalfWidth = (ImGui::GetWindowWidth() - 25) * 0.5;
		if (ImGui::BeginChild("Items", ImVec2(windowHalfWidth, 0))) {
			if (ImGui::BeginTabBar("Tabs")) {
				if (ImGui::BeginTabItem("Textures")) {
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);	
					if (ImGui::BeginChild("TexturesImage", ImVec2(windowHalfWidth, 0), ImGuiChildFlags_Borders)) {
						uint32 i = 0;
						for (auto &iter : TextureManager::textures) {
							if (ImGui::ImageButton(iter.first.c_str(), iter.second.ID, ImVec2(100, 100))) {
								data.selectedItem = iter.first;
								data.selectedItemType = 1;
							}
							ImGui::SameLine();
							i++;
							if (i % (uint32)(std::fmax((windowHalfWidth / 116), 1)) == 0) ImGui::NewLine();
						}
						ImGui::EndChild();
					}
					ImGui::PopStyleVar();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Cubemaps")) {
					if (ImGui::BeginChild("CubemapsImage", ImVec2(windowHalfWidth, 0), ImGuiChildFlags_Borders)) {
						for (auto& iter : CubemapManager::cubemaps) {
							if (ImGui::Selectable(iter.first.c_str(), data.selectedItem == iter.first)) {
								data.selectedItem = iter.first;
								data.selectedItemType = 2;
							}
						}
						ImGui::EndChild();
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		// Properties Section
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("Properties", ImVec2(windowHalfWidth, 0), ImGuiChildFlags_Borders);
		if (data.selectedItem != "") {
			if (data.selectedItemType == 1) {
				Texture* texture = TextureManager::GetTexture(data.selectedItem);
				ImGui::Image(texture->ID, ImVec2(windowHalfWidth - 15, windowHalfWidth - 15));
				ImGui::Text(("Selected Texture: " + data.selectedItem).c_str());
				ImGui::Text(("Texture Path: " + texture->GetTexturePath()).c_str());
				ImGui::Text("Resolution: %ix%i", texture->GetWidth(), texture->GetHeight());
				ImGui::Text("Number of Color Channels: %i", texture->GetNumberOfChannels());
				if (ImGui::Button("Delete")) {
					TextureManager::DeleteTexture(data.selectedItem);
					data.selectedItem = "";
					data.selectedItemType = 0;
				}
			}
			else if (data.selectedItemType == 2) {
				Cubemap* cubemap = CubemapManager::GetCubemap(data.selectedItem);
				ImGui::Text(("Selected Cubemap: " + data.selectedItem).c_str());
				std::vector<String> filepaths = cubemap->GetTexturePaths();
				ImGui::Text(("Right Texture Path: " + filepaths[0]).c_str());
				ImGui::Text(("Left Texture Path: " + filepaths[1]).c_str());
				ImGui::Text(("Top Texture Path: " + filepaths[2]).c_str());
				ImGui::Text(("Bottom Texture Path: " + filepaths[3]).c_str());
				ImGui::Text(("Front Texture Path: " + filepaths[4]).c_str());
				ImGui::Text(("Back Texture Path: " + filepaths[5]).c_str());
				if (ImGui::Button("Delete")) {
					CubemapManager::DeleteCubemap(data.selectedItem);
					data.selectedItem = "";
					data.selectedItemType = 0;
				}
			}

		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	ImGui::End();
}
void GUI::ShowCameraInfoOverlay(GUIContext* context, bool* p_open) {
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 8.0f;
	ImGui::SetNextWindowBgAlpha(0.35f);
	if (ImGui::Begin("Camera Info Overlay", p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Camera Info Overlay");
		ImGui::Text("FPS: %.3f (%.2fms)", ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime * 1000.0f);
		ImGui::Text("Position: (%.3f, %.3f, %.3f)", context->camera->position.x, context->camera->position.y, context->camera->position.z);
		ImGui::Text("Direction: (%.3f, %.3f, %.3f)", context->camera->front.x, context->camera->front.y, context->camera->front.z);
		ImGui::Text("Speed: %.3f", context->camera->speed);
	}
	style.WindowRounding = 0.0f;
	ImGui::End();
}

void GUI::OpenSaveAsPopup(GUIContext* context) {
	if (ImGui::BeginPopupModal("Save As File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::InputText("File Path", &context->shaderFilepath[0], 256);
		ImGui::SetItemDefaultFocus();
		if (ImGui::Button("Save")) {
			File::SaveToFile(context->shaderFilepath, context->editor->GetText());
			data.loadedShaderFilepath = context->shaderFilepath;
			data.recentFiles.push_back(data.loadedShaderFilepath);
			if (data.recentFiles.size() > 5) data.recentFiles.erase(data.recentFiles.begin());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
void GUI::OpenLoadFilePopup(GUIContext* context) {
	if (ImGui::BeginPopupModal("Open File", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::InputText("File Path", &context->shaderFilepath[0], 256);
		ImGui::SetItemDefaultFocus();
		if (ImGui::Button("Load")) {
			context->editor->SetText(File::LoadFromFile(context->shaderFilepath));
			data.loadedShaderFilepath = context->shaderFilepath;
			data.recentFiles.push_back(data.loadedShaderFilepath);
			if (data.recentFiles.size() > 5) data.recentFiles.erase(data.recentFiles.begin());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
void GUI::OpenExitPopup(GUIContext* context) {
	if (ImGui::BeginPopupModal("Exit Popup", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you want to exit?");
		if (ImGui::Button("Yes")) {
			context->window->SetWindowShouldClose(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("No")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void GUI::Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void GUI::Shutdown() {
	String text = "";
	for (auto& filepath : data.recentFiles) {
		text.append(filepath + '\n');
	}
	File::SaveToFile("recent.txt", text);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}