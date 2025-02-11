#pragma once

#include "Cubemap.h"
#include <unordered_map>	
#include <vector>
#include "../Math/Precision.h"
#include "../Renderer/Shader.h"

class CubemapManager {
private:
	CubemapManager() {}
public:
	static std::unordered_map<String, Cubemap> cubemaps;

	static void AddCubemap(String name, Cubemap* cubemap);
	static bool AddCubemap(String name, std::vector<String> cubemapPaths);
	static Cubemap* GetCubemap(String name);
	static void BindCubemap(Shader* shader, String name, uint32 textureUnit);
	static void BindAllCubemaps(Shader* shader);
	static void DeleteCubemap(String name);
	static void DeleteAllCubemaps();
};