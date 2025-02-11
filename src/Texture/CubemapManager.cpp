#include "CubemapManager.h"

std::unordered_map<String, Cubemap> CubemapManager::cubemaps;

void CubemapManager::AddCubemap(String name, Cubemap* cubemap) {
	cubemaps[name] = *cubemap;
}
bool CubemapManager::AddCubemap(String name, std::vector<String> cubemapPaths) {
	bool success;
	Cubemap cubemap(cubemapPaths, &success);
	if (success) cubemaps.insert({ name, cubemap });
	return success;
}
Cubemap* CubemapManager::GetCubemap(String name) {
	return &cubemaps[name];
}
void CubemapManager::BindCubemap(Shader* shader, String name, uint32 textureUnit) {
	cubemaps[name].Bind(textureUnit);
	shader->SetInt(name, textureUnit);
}
void CubemapManager::BindAllCubemaps(Shader* shader) {
	uint32 i = 0;
	for (auto& pair : cubemaps) {
		BindCubemap(shader, pair.first, i);
		i++;
	}
}
void CubemapManager::DeleteCubemap(String name) {
	cubemaps[name].Delete();
	cubemaps.erase(name);
}
void CubemapManager::DeleteAllCubemaps() {
	for (auto& pair : cubemaps) pair.second.Delete();
	cubemaps.clear();
}