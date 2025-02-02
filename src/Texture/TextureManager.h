#pragma once

#include "Texture.h"
#include <unordered_map>	
#include "../Math/Precision.h"
#include "../Renderer/Shader.h"

class TextureManager {
private:
	TextureManager() {}
public:
	static std::unordered_map<String, Texture> textures;

	static void		AddTexture(String name, Texture* texture);
	static bool		AddTexture(String name, String texturePath);
	static Texture* GetTexture(String name);
	static void		BindTexture(Shader* shader, String name, int textureUnit);
	static void		BindAllTextures(Shader* shader);
	static void		DeleteTexture(String name);
	static void		DeleteAllTextures();
};