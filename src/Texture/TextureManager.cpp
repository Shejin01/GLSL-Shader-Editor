#include "TextureManager.h"

std::unordered_map<String, Texture> TextureManager::textures;

void TextureManager::AddTexture(String name, Texture* texture) {
	textures[name] = *texture;
}
bool TextureManager::AddTexture(String name, String texturePath) {
	bool success = false;
	Texture texture = Texture(texturePath.c_str(), &success);
	if (success) textures.insert({ name, texture });
	return success;
}
Texture* TextureManager::GetTexture(String name) {
	return &textures[name];
}
void TextureManager::BindTexture(Shader* shader, String name, int textureUnit) {
	GetTexture(name)->Bind(textureUnit);
	shader->SetInt(name, textureUnit);
}
void TextureManager::BindAllTextures(Shader* shader) {
	int i = 0;
	for (auto& iter : textures) {
		BindTexture(shader, iter.first, i);
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