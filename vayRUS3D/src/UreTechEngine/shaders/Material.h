#pragma once

#include"nlohmann/json.hpp"

#include<UreTechEngine/utils/string/string.h>
#include<UreTechEngine/shaders/vertexStructs.hpp>

class Material {
public:
	texture colorText;
	UreTechEngine::string colorTextPath = "";
	texture normalMap;
	UreTechEngine::string normalMapPath = "";
	bool litRender = true;
	float specularStrength = 0.0f;
};