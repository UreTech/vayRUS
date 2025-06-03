#include "EngineBase.h"
#include<iostream>
#include <windows.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<UreTechEngine/EngineCore.h>

using namespace UreTechEngine;

UreTechEngine::UreTechEngineClass* UreTechEngine::UreTechEngineClass::c_Instance = nullptr;
UreTechEngine::UreTechEngineClass* engRef = nullptr;
unsigned int UreTechEngine::UreTechEngineClass::displayWidth = 1400;
unsigned int UreTechEngine::UreTechEngineClass::displayHeight = 1000;
bool UreTechEngine::UreTechEngineClass::windowMinmized = false;

void SetThreadAffinity(std::thread& t, DWORD_PTR affinityMask) {
	HANDLE threadHandle = t.native_handle();
	SetThreadAffinityMask(threadHandle, affinityMask);
}

void SetThreadPriority(std::thread& t, int priority) {
	HANDLE threadHandle = t.native_handle();
	SetThreadPriority(threadHandle, priority);
}



UreTechEngine::UreTechEngineClass* UreTechEngine::UreTechEngineClass::getEngine()
{
    if (c_Instance != nullptr) {
        return c_Instance;
    }
    else {
        c_Instance = new UreTechEngineClass();
		engRef = c_Instance;
		if (UPK_ENABLE_PACKAGE_SYSTEM) {
			c_Instance->init_upk_system(UPK_PACKAGE_PATH, UPK_PACKAGE_ENC_KEY);
		}

		//openGL init
		if (!glfwInit()) {
			EngineConsole::log("GLFW ERROR!", EngineConsole::ERROR_FATAL);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		c_Instance->window = glfwCreateWindow(displayWidth, displayHeight, ENGINE_WINDOW_TITLE, NULL, NULL);
		if (c_Instance->window == NULL) {
			glfwTerminate();
			EngineConsole::log("WINDOW ERROR!", EngineConsole::ERROR_FATAL);
			while (1) {}
		}
		
		
		glfwMakeContextCurrent(c_Instance->window);
		/*
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			EngineConsole::log("GLAD ERROR!", EngineConsole::ERROR_FATAL);
		}
		*/

		c_Instance->mainRenderer = new Renderer();
		c_Instance->mainRenderer->window = c_Instance->window;

		c_Instance->mainRenderer->InitVulkan();
		c_Instance->mainRenderer->InitImGuiVulkan();

		c_Instance->mainRenderer->createImageSamplers();
		c_Instance->mainRenderer->VertexShaders.push_back(c_Instance->mainRenderer->createShaderModule("/shaders/vs.spv"));
		c_Instance->mainRenderer->FragmentShaders.push_back(c_Instance->mainRenderer->createShaderModule("/shaders/fs.spv"));
		c_Instance->mainRenderer->setupShaderUniforms();
		c_Instance->mainRenderer->createEnginePipeline();

		

		//EngineConsole::log("HELL YEAH VULKAN!", EngineConsole::ERROR_FATAL);
		//c_Instance->mainRenderer->attachShader("/shaders/baseVS.glsl", GL_VERTEX_SHADER);
		//c_Instance->mainRenderer->attachShader("/shaders/baseFS.glsl", GL_FRAGMENT_SHADER);
		//c_Instance->mainRenderer->link();

		/*
		c_Instance->mainRenderer->addUniform("uMtxModel");
		c_Instance->mainRenderer->addUniform("uMtxProj");
		c_Instance->mainRenderer->addUniform("uMtxCam");
		c_Instance->mainRenderer->addUniform("uPosCam");
		c_Instance->mainRenderer->addUniform("lightPos");
		c_Instance->mainRenderer->addUniform("uLightColor");

		c_Instance->mainRenderer->addUniform("texture0");
		c_Instance->mainRenderer->addUniform("texture1");
		c_Instance->mainRenderer->addUniform("texture2");
		c_Instance->mainRenderer->addUniform("texture3");
		c_Instance->mainRenderer->addUniform("texture4");
		c_Instance->mainRenderer->addUniform("texture5");

		c_Instance->mainRenderer->addUniform("specularStrength0");
		c_Instance->mainRenderer->addUniform("specularStrength1");
		c_Instance->mainRenderer->addUniform("specularStrength2");
		c_Instance->mainRenderer->addUniform("specularStrength3");
		c_Instance->mainRenderer->addUniform("specularStrength4");
		c_Instance->mainRenderer->addUniform("specularStrength5");

		c_Instance->mainRenderer->addUniform("litRender0");
		c_Instance->mainRenderer->addUniform("litRender1");
		c_Instance->mainRenderer->addUniform("litRender2");
		c_Instance->mainRenderer->addUniform("litRender3");
		c_Instance->mainRenderer->addUniform("litRender4");
		c_Instance->mainRenderer->addUniform("litRender5");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
		
		//glfwWindowHint(GLFW_SAMPLES, 4);
		//glEnable(GL_MULTISAMPLE);
		//******
		*/
        c_Instance->defPlayer = new Player;

		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
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

Renderer* UreTechEngine::UreTechEngineClass::getShaderProgram()
{
	return mainRenderer;
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
	string abc(" spawned with id ");
	UreTechEngine::EngineConsole::log(_toSpawn->entName + abc + u64ToHexStr(_toSpawn->entityID), UreTechEngine::EngineConsole::INFO);
	sceneEntities.push_back(_toSpawn);
	UreTechEngine::EngineConsole::log("Ent addr: " + u64ToHexStr((uint64_t)_toSpawn), UreTechEngine::EngineConsole::DEBUG);
	return sceneEntities[sceneEntities.size() - 1];
}

UreTechEngine::entity* UreTechEngine::UreTechEngineClass::getEntityWithIndex(unsigned int _index)
{
	return sceneEntities[_index];
}

UreTechEngine::entity* UreTechEngine::UreTechEngineClass::getEntityWithName(UreTechEngine::string _entName)
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

bool UreTechEngine::UreTechEngineClass::isThisEntNameAvilable(UreTechEngine::string _entName)
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
	glm::vec4 a(1.0f, 1.0f, 1.0f, 1.0f);
	mainRenderer->setVec4("uLightColor", a);
	for (uint64_t i = 0; i < engRef->getCountOfEntity(); i++) {
		engRef->getEntityWithIndex(i)->tick();
		engRef->getEntityWithIndex(i)->updateVisual();
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

bool UreTechEngine::UreTechEngineClass::killEntity(string _entName)
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

void UreTechEngine::UreTechEngineClass::init_upk_system(string path, string encKey)
{
	this->package = new upk_API();
	this->package->setEncryptionKey(encKey.c_str());
	this->package->readAndCreateTree(path.c_str());
	loadedPackages.push_back(this->package);
	EngineConsole::log("Imported main package:\n" + string::stdStrToUStr(this->package->packageInfo()), EngineConsole::t_error::INFO);
}

upk_API* UreTechEngine::UreTechEngineClass::imp_upk_package(string path, string encKey)
{
	upk_API* res = new upk_API();
	this->package->setEncryptionKey(encKey.c_str());
	this->package->readAndCreateTree(path.c_str());
	loadedPackages.push_back(res);
	EngineConsole::log("Imported new package:\n" + string::stdStrToUStr(res->packageInfo()), EngineConsole::t_error::INFO);
	return res;
}

void UreTechEngine::UreTechEngineClass::saveCurrentMap(std::string mapPath)
{
	nlohmann::json map;

	for (int i = 0; i < this->sceneEntities.size(); i++) {
		map["OBJECT" + std::to_string(i)]["CLASS"] = sceneEntities[i]->entClassName.std_str();
		map["OBJECT" + std::to_string(i)]["NAME"] = sceneEntities[i]->entName.std_str();
		map["OBJECT" + std::to_string(i)]["CUSTOM"] = sceneEntities[i]->entCustomSets.std_str();

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
		EngineConsole::log(string("map saved as ") + string::stdStrToUStr(mapPath) + string(".UMAP"), EngineConsole::INFO);
	}
	else {
		EngineConsole::log("can not save the map!", EngineConsole::INFO);
	}

}

void UreTechEngine::UreTechEngineClass::saveGame(std::string gamePath)
{
	nlohmann::json game;
	int mi = 0;
	/*
	for (auto it = loadedMaterials.begin(); it != loadedMaterials.end(); ++it) {
		//  terat r n first ve second  yeleri, anahtar-de er  iftini temsil eder
		Material mat = it->second;
		game["GAME"]["MATERIAL" + std::to_string(mi)]["path"] = it->first+".UMAT";
		mi++;
	}
	*/
	std::ofstream file(gamePath + ".UGAME");
	if (file.is_open()) {
		file << std::setw(4) << game << std::endl;
		file.close();
		EngineConsole::log(string("map saved as ") + string::stdStrToUStr(gamePath) + string(".UGAME"), EngineConsole::INFO);
	}
	else {
		EngineConsole::log("can not save the GAME!", EngineConsole::WARN);
	}
}

void UreTechEngine::UreTechEngineClass::loadGame(std::string gamePath)
{
	std::ifstream file(gamePath+".UGAME");


	if (!file.is_open()) {
		EngineConsole::log(string("game loading error: ") + string::stdStrToUStr(gamePath) + string(".UGAME"), EngineConsole::INFO);
		return;
	}

	if (!file.good()) {
		EngineConsole::log(string("game loading error(buffer error): ") + string::stdStrToUStr(gamePath) + string(".UGAME"), EngineConsole::INFO);
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
				UreTechEngine::string loadingMat = string::stdStrToUStr(jsonData["GAME"][finding]["path"]);
				Material lodMat;
				//lodMat.loadMaterial(loadingMat);
				//loadedMaterials[loadingMat.c_str()] = lodMat;
				i++;
			}
			else {
				break;
			}
		}
		else {
			EngineConsole::log(string("game loading error(file is not valid): ") + string::stdStrToUStr(gamePath) + string(".UGAME"), EngineConsole::ERROR_ERROR);
			break;
		}
	}
	EngineConsole::log(string("game loaded: ") + string::stdStrToUStr(gamePath) + string(".UGAME") + string::stdStrToUStr(std::to_string(i)), EngineConsole::INFO);
}

UreTechEngine::UreTechEngineClass::UreTechEngineClass()
{
}

UreTechEngine::UreTechEngineClass::~UreTechEngineClass()
{
}

