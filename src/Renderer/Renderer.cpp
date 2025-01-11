#include "Renderer.h"

void Renderer::Clear(glm::vec4 color) {
	Clear(color.r, color.g, color.b, color.a);
}
void Renderer::Clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}