#include "Framebuffer.h"

FBO::FBO() : width(0), height(0) {}
FBO::FBO(int32 width, int32 height) : width(width), height(height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	texture = Texture(nullptr, width, height, 3);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.ID, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		Logger::LogLine("Framebuffer", "Framebuffer is not complete.");
		Delete();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
uint32 FBO::GetTextureID() const {
	return texture.ID;
}
void FBO::Bind() {
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}
void FBO::BindTexture(int32 textureUnit) {
	texture.Bind(textureUnit);
}
void FBO::Unbind(glm::vec2 defaultSize) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, defaultSize.x, defaultSize.y);
}
void FBO::UnbindTexture() {
	texture.Unbind();
}
void FBO::Delete() {
	glDeleteFramebuffers(1, &ID);
}