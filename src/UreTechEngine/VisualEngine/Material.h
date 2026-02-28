#pragma once

#include"nlohmann/json.hpp"

#include<UreTechEngine/utils/string/string.h>
#include<UreTechEngine/VisualEngine/vertexStructs.hpp>

class Material {
public:
	texture* colorText = nullptr;
	UreTechEngine::string colorTextPath = "";
	texture* normalMap = nullptr;
	UreTechEngine::string normalMapPath = "";
	bool litRender = true;
	float specularStrength = 0.0f;
};