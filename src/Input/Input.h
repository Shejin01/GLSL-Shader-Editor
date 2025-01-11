#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Math/Precision.h"
#include "../Window/Window.h"
#include <glm/glm.hpp>
#include <vector>

class Input {
public:
	struct KeyData {
		bool prevPressedKeys[348];
		bool pressedKeys[348];
	};
	struct MouseData {
		bool prevPressedButtons[8];
		bool pressedButtons[8];
		glm::vec2 cursorPos;
		float scrollOffset;
	};
private:
	KeyData keyData;
	MouseData mouseData;
	Window* window;

	static std::vector<Input*> instances;
	static void key_callback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
	static void mouse_button_callback(GLFWwindow* window, int32 button, int32 action, int32 mods);
	static void cursor_move_callback(GLFWwindow* window, double xPos, double yPos);
	static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

	void UpdateCursorPos(double x, double y);
public:
	Input(Window* window);
	void PollWindow(Window* window);
	void PollEvents();
	void UpdateEvents();

	void SetKey(int32 key, bool value);
	bool GetKeyPressed(int32 key) const;
	bool GetKeyHeld(int32 key) const;
	bool GetKeyReleased(int32 key) const;

	void SetMouseButton(int32 button, bool value);
	bool GetMouseButtonPressed(int32 button) const;
	bool GetMouseButtonHeld(int32 button) const;
	bool GetMouseButtonReleased(int32 button) const;

	void SetCursorPos(glm::vec2 position);
	void SetCursorPos(double x, double y);
	glm::vec2 GetCursorPos() const;

	void SetScrollOffset(double offset);
	double GetScrollOffset() const;
};