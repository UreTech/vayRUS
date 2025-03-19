#include "Material.h"

#include"nlohmann/json.hpp"
#include<iostream>
#include<fstream>

#include<../EngineCore.h>

using namespace UreTechEngine;

void Material::saveMaterial(string savePath)
{
	nlohmann::json mat;
	mat["Material"]["Texture Path"] = colorTextPath.std_str();
	mat["Material"]["Normal Map Path"] = normalMapPath.std_str();
	mat["Material"]["Lit"] = litRender;
	mat["Material"]["Specular"] = specularStrength;

	std::ofstream matf((savePath + ".UMAT").std_str());
	if (matf.is_open()) {
		matf << std::setw(4) << mat << std::endl;
		matf.close();
		EngineConsole::log("material saved as " + savePath + ".UMAT", EngineConsole::INFO_NORMAL);
	}
	else {
		EngineConsole::log("can not save the material!", EngineConsole::WARN_NORMAL);
	}
}

void Material::loadMaterial(string savePath)
{
	std::ifstream file(savePath.std_str());

	nlohmann::json jsonData;
	file >> jsonData;
	file.close();

	UreTechEngineClass* eng = UreTechEngineClass::getEngine();

	this->colorTextPath = jsonData["Material"]["Texture Path"];

	auto it = eng->loadedTextures.find(this->colorTextPath.std_str());

	if (it != eng->loadedTextures.end()) {
		this->colorText = it->second;
	}
	else {
		texture lodTex = TextureManager::getInstance()->loadTextureFromFile(this->colorTextPath);
		eng->loadedTextures[this->colorTextPath.std_str()] = lodTex;
	}

	this->normalMapPath = jsonData["Material"]["Normal Map Path"];

	auto it2 = eng->loadedTextures.find(this->normalMapPath.std_str());

	if (it2 != eng->loadedTextures.end()) {
		this->normalMap = it2->second;
	}
	else {
		texture lodTex = TextureManager::getInstance()->loadTextureFromFile(this->normalMapPath);
		eng->loadedTextures[this->normalMapPath.std_str()] = lodTex;
	}

	this->litRender = jsonData["Material"]["Lit"];
	this->specularStrength = jsonData["Material"]["Specular"];
}
