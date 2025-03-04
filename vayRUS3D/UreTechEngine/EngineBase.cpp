#include "EngineBase.h"
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<../EngineCore.h>

using namespace UreTechEngine;

UreTechEngine::UreTechEngineClass* UreTechEngine::UreTechEngineClass::c_Instance = nullptr;
unsigned int UreTechEngine::UreTechEngineClass::displayWidth = 1000;
unsigned int UreTechEngine::UreTechEngineClass::displayHeight = 1000;

UreTechEngine::UreTechEngineClass* UreTechEngine::UreTechEngineClass::getEngine()
{
    if (c_Instance != nullptr) {
        return c_Instance;
    }
    else {
        c_Instance = new UreTechEngineClass();

		//openGL init
		if (!glfwInit()) {
			std::cout << "GL ERROR!";
			while(1){}
		}

		c_Instance->window = glfwCreateWindow(displayWidth, displayHeight, ENGINE_WINDOW_TITLE, NULL, NULL);
		if (c_Instance->window == NULL) {
			std::cout << "WINDOW ERROR!";
			glfwTerminate();
			while (1) {}
		}
		glfwMakeContextCurrent(c_Instance->window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "GLAD ERROR!";
			while (1) {}
		}

		c_Instance->mainShaderProgram = new ShaderProgram();

		c_Instance->mainShaderProgram->attachShader("content/shaders/baseVS.glsl", GL_VERTEX_SHADER);
		c_Instance->mainShaderProgram->attachShader("content/shaders/baseFS.glsl", GL_FRAGMENT_SHADER);
		c_Instance->mainShaderProgram->link();

		c_Instance->mainShaderProgram->addUniform("uTranslation");
		c_Instance->mainShaderProgram->addUniform("uRotation");
		c_Instance->mainShaderProgram->addUniform("uScale");
		c_Instance->mainShaderProgram->addUniform("uMtxProj");
		c_Instance->mainShaderProgram->addUniform("uMtxCamPos");
		c_Instance->mainShaderProgram->addUniform("uCamRot");
		c_Instance->mainShaderProgram->addUniform("uMtxCam");
		c_Instance->mainShaderProgram->addUniform("lightPos");
		c_Instance->mainShaderProgram->addUniform("uLightColor");

		c_Instance->mainShaderProgram->addUniform("texture0");
		c_Instance->mainShaderProgram->addUniform("texture1");
		c_Instance->mainShaderProgram->addUniform("texture2");
		c_Instance->mainShaderProgram->addUniform("texture3");
		c_Instance->mainShaderProgram->addUniform("texture4");
		c_Instance->mainShaderProgram->addUniform("texture5");

		c_Instance->mainShaderProgram->addUniform("specularStrength0");
		c_Instance->mainShaderProgram->addUniform("specularStrength1");
		c_Instance->mainShaderProgram->addUniform("specularStrength2");
		c_Instance->mainShaderProgram->addUniform("specularStrength3");
		c_Instance->mainShaderProgram->addUniform("specularStrength4");
		c_Instance->mainShaderProgram->addUniform("specularStrength5");

		c_Instance->mainShaderProgram->addUniform("litRender0");
		c_Instance->mainShaderProgram->addUniform("litRender1");
		c_Instance->mainShaderProgram->addUniform("litRender2");
		c_Instance->mainShaderProgram->addUniform("litRender3");
		c_Instance->mainShaderProgram->addUniform("litRender4");
		c_Instance->mainShaderProgram->addUniform("litRender5");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
		
		//glfwWindowHint(GLFW_SAMPLES, 4);
		//glEnable(GL_MULTISAMPLE);
		//******

        c_Instance->defPlayer = new Player;
        return c_Instance;
    }
}

void UreTechEngine::UreTechEngineClass::setKeyCallBackFunc(GLFWkeyfun func,GLFWmousebuttonfun funcM)
{
	glfwSetKeyCallback(c_Instance->window, func);
	glfwSetMouseButtonCallback(c_Instance->window, funcM);
}

UreTechEngine::Player* UreTechEngine::UreTechEngineClass::getPlayer()
{
    return defPlayer;
}

ShaderProgram* UreTechEngine::UreTechEngineClass::getShaderProgram()
{
	return mainShaderProgram;
}

GLFWwindow* UreTechEngine::UreTechEngineClass::getWindow()
{
	return window;
}

UreTechEngine::entity* UreTechEngine::UreTechEngineClass::spawnEntity(entity* _toSpawn)
{
	_toSpawn->entityID = lastID;
	lastID++;
	_toSpawn->init(this);
	_toSpawn->begin();
	UreTechEngine::EngineConsole::log(uStr(_toSpawn->entName) + uStr(" spawned with id ") + intToHex(_toSpawn->entityID), UreTechEngine::EngineConsole::INFO_NORMAL);
	sceneEntities.push_back(_toSpawn);
	return sceneEntities[sceneEntities.size() - 1];
}

UreTechEngine::entity* UreTechEngine::UreTechEngineClass::getEntityWithIndex(unsigned int _index)
{
	return sceneEntities[_index];
}

UreTechEngine::entity* UreTechEngine::UreTechEngineClass::getEntityWithName(std::string _entName)
{
	for (int i = 0; i < sceneEntities.size(); i++) {
		if (sceneEntities[i]->entName == _entName) {
			return sceneEntities[i];
		}
	}
	return nullptr;
}

entity* UreTechEngine::UreTechEngineClass::getEntityWithID(uint64_t id)
{
	for (uint64_t i = 0; i < sceneEntities.size(); i++) {
		if (sceneEntities[i]->entityID == id) {
			return sceneEntities[i];
		}
	}
	return nullptr;
}

bool UreTechEngine::UreTechEngineClass::isThisEntNameAvilable(std::string _entName)
{
	for (int i = 0; i < sceneEntities.size(); i++) {
		if (sceneEntities[i]->entName == _entName) {
			return false;
		}
	}
	return true;
}

bool UreTechEngine::UreTechEngineClass::isValidEntity(uint64_t id)
{
	for (uint64_t i = 0; i < sceneEntities.size(); i++) {
		if (sceneEntities[i]->entityID == id) {
			return true;
		}
	}
	return false;
}

unsigned int UreTechEngine::UreTechEngineClass::getCountOfEntity()
{
	return sceneEntities.size();
}

void UreTechEngine::UreTechEngineClass::engineTick()
{
	for (int i = 0; i < sceneEntities.size(); i++) {
		if (sceneEntities[i] != nullptr) {
			glm::vec4 a(1.0f, 1.0f, 1.0f, 1.0f);
			mainShaderProgram->setVec4("uLightColor", a);
			sceneEntities[i]->updateVisual();
		}
		else {
			EngineConsole::log("Invalid entity update!", EngineConsole::ERROR_NORMAL);
		}
	}
}

bool UreTechEngine::UreTechEngineClass::killEntity(entity* _ent)
{
	for (uint64_t i = 0; i < sceneEntities.size(); i++) {
		if (_ent == sceneEntities[i]) {
			free(sceneEntities[i]);
			sceneEntities.remove(i);
			return true;
		}
	}
	EngineConsole::log("Invalid entity kill attemp!", EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
	return false;
}

bool UreTechEngine::UreTechEngineClass::killEntity(std::string _entName)
{
	for (uint64_t i = 0; i < sceneEntities.size(); i++) {
		if (_entName == sceneEntities[i]->entName) {
			free(sceneEntities[i]);
			sceneEntities.remove(i);
			return true;
		}
	}
	EngineConsole::log("Invalid entity kill attemp!", EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
	return false;
}

void UreTechEngine::UreTechEngineClass::saveCurrentMap(std::string mapPath)
{
	nlohmann::json map;

	for (int i = 0; i < this->sceneEntities.size(); i++) {
		map["OBJECT" + std::to_string(i)]["CLASS"] = sceneEntities[i]->entClassName;
		map["OBJECT" + std::to_string(i)]["NAME"] = sceneEntities[i]->entName;
		map["OBJECT" + std::to_string(i)]["CUSTOM"] = sceneEntities[i]->entCustomSets;

		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["LOCX"] = sceneEntities[i]->transform.Location.x;
		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["LOCY"] = sceneEntities[i]->transform.Location.y;
		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["LOCZ"] = sceneEntities[i]->transform.Location.z;
					 
 		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["ROTR"] = sceneEntities[i]->transform.Rotation.roll;
		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["ROTP"] = sceneEntities[i]->transform.Rotation.pitch;
		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["ROTY"] = sceneEntities[i]->transform.Rotation.yaw;
					 
 		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["SCLX"] = sceneEntities[i]->transform.Scale.x;
		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["SCLY"] = sceneEntities[i]->transform.Scale.y;
		map["OBJECT" + std::to_string(i)]["TRANSFORM"]["SCLZ"] = sceneEntities[i]->transform.Scale.z;
	}

	std::ofstream file(mapPath + ".UMAP");
	if (file.is_open()) {
		file << std::setw(4) << map << std::endl;
		file.close();
		EngineConsole::log(uStr("map saved as ") + mapPath + uStr(".UMAP"), EngineConsole::INFO_NORMAL);
	}
	else {
		EngineConsole::log("can not save the map!", EngineConsole::WARN_NORMAL);
	}

}

void UreTechEngine::UreTechEngineClass::saveGame(std::string gamePath)
{
	nlohmann::json game;
	int mi = 0;
	for (auto it = loadedMaterials.begin(); it != loadedMaterials.end(); ++it) {
		// Ýteratörün first ve second üyeleri, anahtar-deðer çiftini temsil eder
		Material mat = it->second;
		game["GAME"]["MATERIAL" + std::to_string(mi)]["path"] = it->first+".UMAT";
		mi++;
	}

	std::ofstream file(gamePath + ".UGAME");
	if (file.is_open()) {
		file << std::setw(4) << game << std::endl;
		file.close();
		EngineConsole::log(uStr("map saved as ") + gamePath + uStr(".UGAME"), EngineConsole::INFO_NORMAL);
	}
	else {
		EngineConsole::log("can not save the GAME!", EngineConsole::WARN_NORMAL);
	}
}

void UreTechEngine::UreTechEngineClass::loadGame(std::string gamePath)
{
	std::ifstream file(gamePath+".UGAME");


	if (!file.is_open()) {
		EngineConsole::log(uStr("game loading error: ") + gamePath + uStr(".UGAME"), EngineConsole::ERROR_ERROR);
		return;
	}

	if (!file.good()) {
		EngineConsole::log(uStr("game loading error(buffer error): ") + gamePath + uStr(".UGAME"), EngineConsole::ERROR_ERROR);
		return;
	}

	nlohmann::json jsonData;
	file.seekg(0);
	file >> jsonData;
	file.close();
	unsigned int i = 0;

	while (1) {
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		std::string finding = "MATERIAL" + std::to_string(i);
		std::string asd = jsonData.dump();

		if (jsonData.find("GAME") != jsonData.end()) {
			auto& inner_json = jsonData["GAME"];
			if (inner_json.find(finding) != inner_json.end()) {
				std::string laodingMat = jsonData["GAME"][finding]["path"];
				Material lodMat;
				lodMat.loadMaterial(laodingMat);
				loadedMaterials[laodingMat] = lodMat;
				i++;
			}
			else {
				break;
			}
		}
		else {
			EngineConsole::log(uStr("game loading error(file is not valid): ") + gamePath + uStr(".UGAME"), EngineConsole::ERROR_ERROR);
			break;
		}
	}
	EngineConsole::log(uStr("game loaded: ") + gamePath + uStr(".UGAME") + std::to_string(i), EngineConsole::INFO_NORMAL);
}

UreTechEngine::UreTechEngineClass::UreTechEngineClass()
{
}

UreTechEngine::UreTechEngineClass::~UreTechEngineClass()
{
}


