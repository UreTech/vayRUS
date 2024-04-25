#pragma once

#include"nlohmann/json.hpp"

typedef unsigned int texture;

class Material {
public:
	texture colorText;
	std::string colorTextPath = "";
	texture normalMap;
	std::string normalMapPath = "";
	bool litRender = true;
	float specularStrength = 0.0f;
	void saveMaterial(std::string savePath);
	void loadMaterial(std::string savePath);
};