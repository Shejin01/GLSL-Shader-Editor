#include "TextureManager.h"

std::unordered_map<String, Texture> TextureManager::textures;

void TextureManager::AddTexture(String name, Texture* texture) {
	textures[name] = *texture;
}
bool TextureManager::AddTexture(String name, String texturePath) {
	bool success;
	Texture texture(texturePath.c_str(), &success);
	if (success) textures.insert({ name, texture });
	return success;
}
Texture* TextureManager::GetTexture(String name) {
	return &textures[name];
}
void TextureManager::BindTexture(Shader* shader, String name, uint32 textureUnit) {
	textures[name].Bind(textureUnit);
	shader->SetInt(name, textureUnit);
}
void TextureManager::BindAllTextures(Shader* shader) {
	uint32 i = 0;
	for (auto& pair : textures) {
		BindTexture(shader, pair.first, i);
		i++;
	}
}
void TextureManager::DeleteTexture(String name) {
	textures[name].Delete();
	textures.erase(name);
}
void TextureManager::DeleteAllTextures() {
	for (auto& pair : textures) pair.second.Delete();
	textures.clear();
}