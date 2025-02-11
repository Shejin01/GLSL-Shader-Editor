#include "Cubemap.h"

Cubemap::Cubemap() {}
Cubemap::Cubemap(std::vector<String> texturePaths, bool* success) : texturePaths(texturePaths) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int32 width, height, nrChannels;
	if (success != nullptr) *success = true;
	for (int i = 0; i < 6; i++) {
		unsigned char* data = stbi_load(texturePaths[i].c_str(), &width, &height, &nrChannels, 0);
		if (data != nullptr) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB + nrChannels / 4, GL_UNSIGNED_BYTE, data);
		}
		else {
			Logger::LogLine("Cubemap", "Failed to load texture from path: " + texturePaths[i]);
			if (success != nullptr) *success = false;
		}
		stbi_image_free(data);
	}
}

std::vector<String> Cubemap::GetTexturePaths() const {
	return texturePaths;
}

void Cubemap::Bind(uint32 textureUnit) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}
void Cubemap::Unbind() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void Cubemap::Delete() {
	glDeleteTextures(1, &ID);
}