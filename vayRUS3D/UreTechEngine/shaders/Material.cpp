#include "Material.h"

#include"nlohmann/json.hpp"
#include<iostream>
#include<fstream>

#include<../EngineCore.h>

using namespace UreTechEngine;

void Material::saveMaterial(std::string savePath)
{
	nlohmann::json mat;
	mat["Material"]["Texture Path"] = colorTextPath;
	mat["Material"]["Normal Map Path"] = normalMapPath;
	mat["Material"]["Lit"] = litRender;
	mat["Material"]["Specular"] = specularStrength;

	std::ofstream matf(savePath + ".UMAT");
	if (matf.is_open()) {
		matf << std::setw(4) << mat << std::endl;
		matf.close();
		EngineConsole::consoleError("material saved as " + savePath + ".UMAT", EngineConsole::INFO_NORMAL);
	}
	else {
		EngineConsole::consoleError("can not save the material!", EngineConsole::WARN_NORMAL);
	}
}

void Material::loadMaterial(std::string savePath)
{
	std::ifstream file(savePath);

	nlohmann::json jsonData;
	file >> jsonData;
	file.close();

	UreTechEngineClass* eng = UreTechEngineClass::getEngine();

	this->colorTextPath = jsonData["Material"]["Texture Path"];

	auto it = eng->loadedTextures.find(this->colorTextPath);

	if (it != eng->loadedTextures.end()) {
		this->colorText = it->second;
	}
	else {
		texture lodTex = TextureManager::getInstance()->loadTextureFromFile(this->colorTextPath);
		eng->loadedTextures[this->colorTextPath] = lodTex;
	}

	this->normalMapPath = jsonData["Material"]["Normal Map Path"];

	auto it2 = eng->loadedTextures.find(this->normalMapPath);

	if (it2 != eng->loadedTextures.end()) {
		this->normalMap = it2->second;
	}
	else {
		texture lodTex = TextureManager::getInstance()->loadTextureFromFile(this->normalMapPath);
		eng->loadedTextures[this->normalMapPath] = lodTex;
	}

	this->litRender = jsonData["Material"]["Lit"];
	this->specularStrength = jsonData["Material"]["Specular"];
}
