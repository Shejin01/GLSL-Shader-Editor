#pragma once

#include "../Window/Input.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <imgui/imgui.h>

class Camera {
private:
	glm::vec2 lastCursorPos;
	double pitch = 0.0f, yaw = -90.0f;
	bool cursorDisabled = false;

	void ProcessKeyboard(Input* input, float dt);
	void ProcessMouseMove(Input* input);
	void ProcessScroll(Input* input);
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	float speed = 10.0f;
	float sensitivity = 0.1f;

	bool firstMouse = true;

	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);
	glm::mat4 GetViewMatrix() const;
	void ProcessInputs(Input* input, float dt);
};