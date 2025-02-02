#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
	: position(position), front(front), up(up) {}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(position, position + front, up);
}
void Camera::ProcessInputs(Input* input, float dt) {
	if (input->GetKeyPressed(GLFW_KEY_C) && !(ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse)) {
		if (!cursorDisabled) {
			input->SetCursorMode(GLFW_CURSOR_DISABLED);
			cursorDisabled = true;
			firstMouse = true;
		}
		else {	
			input->SetCursorMode(GLFW_CURSOR_NORMAL);
			cursorDisabled = false;
		}
	}
	if (cursorDisabled) {
		ProcessKeyboard(input, dt);
		ProcessMouseMove(input);
		ProcessScroll(input);
	}
}

void Camera::ProcessKeyboard(Input* input, float dt) {
	float delta = speed * dt;

	if (input->GetKeyHeld(GLFW_KEY_W))
		position += front * delta;
	if (input->GetKeyHeld(GLFW_KEY_S))
		position -= front * delta;
	if (input->GetKeyHeld(GLFW_KEY_A))
		position += glm::normalize(glm::cross(up, front)) * delta;
	if (input->GetKeyHeld(GLFW_KEY_D))
		position -= glm::normalize(glm::cross(up, front)) * delta;
	if (input->GetKeyHeld(GLFW_KEY_SPACE))
		position += up * delta;
	if (input->GetKeyHeld(GLFW_KEY_LEFT_SHIFT))
		position -= up * delta;
}

void Camera::ProcessMouseMove(Input* input) {
	glm::vec2 cursorPos = input->GetCursorPos();
	if (firstMouse) {
		lastCursorPos = cursorPos;
		firstMouse = false;
	}

	glm::vec2 offset = cursorPos - lastCursorPos;
	lastCursorPos = cursorPos;
	offset.y *= -1;
	offset *= sensitivity;

	yaw += offset.x;
	pitch += offset.y;

	pitch = std::fmin(89.9f, std::fmax(-89.9f, pitch));

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(direction);
}

void Camera::ProcessScroll(Input* input) {
	speed += input->GetScrollOffset();
	speed = std::fmin(100.0f, std::fmax(0.1f, speed));
}