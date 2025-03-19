#pragma once

#include"nlohmann/json.hpp"

#include<../UreTechEngine/utils/string/string.h>
typedef unsigned int texture;

class Material {
public:
	texture colorText;
	UreTechEngine::string colorTextPath = "";
	texture normalMap;
	UreTechEngine::string normalMapPath = "";
	bool litRender = true;
	float specularStrength = 0.0f;
	void saveMaterial(UreTechEngine::string savePath);
	void loadMaterial(UreTechEngine::string savePath);
};