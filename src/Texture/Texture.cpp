#include "Texture.h"

Texture::Texture(unsigned char* pixels, int32 width, int32 height, int32 nrChannels) 
	: width(width), height(height), nrChannels(nrChannels) {
	CreateTexture(pixels, width, height, nrChannels);
}
Texture::Texture(const char* texturePath) {
	/*unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data) CreateTexture(data, width, height, nrChannels);
	else Logger::LogLine("Texture", String("Failed to load texture from path: ") + texturePath);
	stbi_image_free(data);*/

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		Logger::LogLine("Texture", String("Failed to load texture from path: ") + texturePath);
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::CreateTexture(unsigned char* pixels, int32 width, int32 height, int32 nrChannels) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::Bind(uint32 textureUnit) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, ID);
}
void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::Delete() {
	glDeleteTextures(1, &ID);
}