#pragma once

#include "../Window/Input.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <math.h>

class Camera {
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	double pitch = 0.0f, yaw = -90.0f;
	float speed = 10.0f;
	float sensitivity = 0.1f;
	bool firstMouse = true;

	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 GetViewMatrix() const;
	void ProcessInputs(Input* input, float dt);
private:
	glm::vec2 lastCursorPos;
	bool cursorDisabled = false;

	void ProcessKeyboard(Input* input, float dt);
	void ProcessMouseMove(Input* input);
	void ProcessScroll(Input* input);
};