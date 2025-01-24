#include "Renderer.h"

void Renderer::Clear(glm::vec4 color) {
	Clear(color.r, color.g, color.b, color.a);
}
void Renderer::Clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

Renderer::Renderer() {
	std::vector<float> quadVertices = {
		-1.0f, -1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 0.0f,
		 1.0f,  1.0f,	1.0f, 1.0f,

		 1.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,	0.0f, 0.0f
	};

	quadVao.Bind();
	quadVbo = VBO(&quadVertices);
	quadVbo.Bind();
	quadVao.LinkAttrib(0, 2, 4 * sizeof(float), 0);
	quadVao.LinkAttrib(1, 2, 4 * sizeof(float), 2 * sizeof(float));
	quadVao.Unbind();
	quadVbo.Unbind();
}

void Renderer::DrawQuad() {
	quadVao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	quadVao.Unbind();
}

Renderer::~Renderer() {
	quadVao.Delete();
	quadVbo.Delete();
}