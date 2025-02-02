#include "Input.h"

std::vector<Input*> Input::instances;

void Input::key_callback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods) {
	for (Input* instance : instances) {
		if (action == GLFW_PRESS) instance->SetKey(key, true);
		else if (action == GLFW_RELEASE) instance->SetKey(key, false);
	}
}
void Input::mouse_button_callback(GLFWwindow* window, int32 button, int32 action, int32 mods) {
	for (Input* instance : instances) {
		if (action == GLFW_PRESS) instance->SetMouseButton(button, true);
		else if (action == GLFW_RELEASE) instance->SetMouseButton(button, false);
	}
}
void Input::cursor_move_callback(GLFWwindow* window, double xPos, double yPos) {
	for (Input* instance : instances) {
		instance->UpdateCursorPos(xPos, yPos);
	}
}
void Input::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	for (Input* instance : instances) {
		instance->SetScrollOffset(yOffset);
	}
}

Input::Input() {}
void Input::CreateInstance(Window* window) {
	this->window = window;
	instances.push_back(this);
	PollWindow(window);
}
void Input::PollWindow(Window* window) {
	glfwSetKeyCallback(window->ID, key_callback);
	glfwSetMouseButtonCallback(window->ID, mouse_button_callback);
	glfwSetCursorPosCallback(window->ID, cursor_move_callback);
	glfwSetScrollCallback(window->ID, scroll_callback);
}
void Input::PollEvents() {
	glfwPollEvents();
}
void Input::UpdateEvents() {
	mouseData.scrollOffset = 0.0;
	mouseData.prevCursorPos = mouseData.cursorPos;
	for (int i = 0; i < 348; i++) {
		keyData.prevPressedKeys[i] = keyData.pressedKeys[i];
	}
	for (int i = 0; i < 8; i++) {
		mouseData.prevPressedButtons[i] = mouseData.pressedButtons[i];
	}
}

void Input::SetKey(int32 key, bool value) {
	keyData.pressedKeys[key] = value;
}
bool Input::GetKeyPressed(int32 key) const {
	return (keyData.prevPressedKeys[key] == false && keyData.pressedKeys[key] == true);
}
bool Input::GetKeyHeld(int32 key) const {
	return keyData.pressedKeys[key];
}
bool Input::GetKeyReleased(int32 key) const {
	return (keyData.prevPressedKeys[key] == true && keyData.pressedKeys[key] == false);
}

void Input::SetMouseButton(int32 button, bool value) {
	mouseData.pressedButtons[button] = value;
}
bool Input::GetMouseButtonPressed(int32 button) const {
	return (mouseData.prevPressedButtons[button] == false && mouseData.pressedButtons[button] == true);
}
bool Input::GetMouseButtonHeld(int32 button) const {
	return mouseData.pressedButtons[button];
}
bool Input::GetMouseButtonReleased(int32 button) const {
	return (mouseData.prevPressedButtons[button] == true && mouseData.pressedButtons[button] == false);
}

void Input::UpdateCursorPos(double x, double y) {
	mouseData.cursorPos.x = x;
	mouseData.cursorPos.y = y;
}
void Input::SetCursorPos(glm::vec2 position) {
	SetCursorPos(position.x, position.y);
}
void Input::SetCursorPos(double x, double y) {
	UpdateCursorPos(x, y);
	glfwSetCursorPos(window->ID, x, y);
}
glm::vec2 Input::GetCursorPos() const {
	return mouseData.cursorPos;
}
bool Input::isMouseMoved() const {
	glm::vec2 offset = mouseData.cursorPos - mouseData.prevCursorPos;
	return (offset == glm::vec2(0.0, 0.0)) ? false : true;
}

void Input::SetScrollOffset(double offset) {
	mouseData.scrollOffset = offset;
	mouseData.scrollAccumulator += offset;
}
double Input::GetScrollOffset() const {
	return mouseData.scrollOffset;
}
void Input::SetScrollAmount(double scrollAmount) {
	mouseData.scrollAccumulator = scrollAmount;
}
float Input::GetScrollAmount() const {
	return mouseData.scrollAccumulator;
}

void Input::SetCursorMode(int value) {
	glfwSetInputMode(window->ID, GLFW_CURSOR, value);
}
int Input::GetCursorMode() const {
	return glfwGetInputMode(window->ID, GLFW_CURSOR);
}