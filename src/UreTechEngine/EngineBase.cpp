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
bool UreTechEngineClass::externalConsoleState = false;

void SetThreadAffinity(std::thread& t, DWORD_PTR affinityMask) {
	HANDLE threadHandle = t.native_handle();
	SetThreadAffinityMask(threadHandle, affinityMask);
}

void SetThreadPriority(std::thread& t, int priority) {
	HANDLE threadHandle = t.native_handle();
	SetThreadPriority(threadHandle, priority);
}

void multiThreadVisualWorker(int workerID, jobPtr job, size_t jobCount) {
	for(size_t i = 0; i < jobCount; i++) {
		entity* toUpdate = *((entity**)job + i);
		if (toUpdate->status == READY) {
			toUpdate->updateVisual();
		}
	}
}

void multiThreadVisualPreWorkers() {
	// ...
}

void multiThreadVisualPostWorkers() {
	// ...
}

void multiThreadVisualPreJob(int id) {
	vkBeginCommandBuffer(engRef->threadCommandBuffers[engRef->mainRenderer->currentFrameIndex][id], &engRef->mainRenderer->multiThreadSecondaryCmdBufferBeginInfo);

	vkCmdBindPipeline(engRef->threadCommandBuffers[engRef->mainRenderer->currentFrameIndex][id], VK_PIPELINE_BIND_POINT_GRAPHICS, engRef->mainRenderer->graphicsPipeline); // attach shaders etc..

	// set viewport & scissor
	int dispW, dispH;
	glfwGetFramebufferSize(engRef->mainRenderer->window, &dispW, &dispH);
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)dispW;
	viewport.height = (float)dispH;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = { (unsigned int)dispW, (unsigned int)dispH };

	vkCmdSetViewport(engRef->threadCommandBuffers[engRef->mainRenderer->currentFrameIndex][id], 0, 1, &viewport);
	vkCmdSetScissor(engRef->threadCommandBuffers[engRef->mainRenderer->currentFrameIndex][id], 0, 1, &scissor);

}

void multiThreadVisualPostJob(int id) {
	vkEndCommandBuffer(engRef->threadCommandBuffers[engRef->mainRenderer->currentFrameIndex][id]);
}

int UreTechEngine::initEngine(){
	UreTechEngineClass::c_Instance = new UreTechEngineClass();

	engRef = UreTechEngineClass::c_Instance;
	if (UPK_ENABLE_PACKAGE_SYSTEM) {
		UreTechEngineClass::c_Instance->init_upk_system(UPK_PACKAGE_PATH, UPK_PACKAGE_ENC_KEY);
	}

	if (!glfwInit()) {
		EngineConsole::log("GLFW ERROR!", EngineConsole::ERROR_FATAL);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

	// displayWidth = 1400
	// displayHeight = 1000
	UreTechEngineClass::c_Instance->window = glfwCreateWindow(UreTechEngineClass::displayWidth, UreTechEngineClass::displayHeight, ENGINE_WINDOW_TITLE, NULL, NULL);
	if (UreTechEngineClass::c_Instance->window == NULL) {
		glfwTerminate();
		EngineConsole::log("WINDOW ERROR!", EngineConsole::ERROR_FATAL);
		while (1) {}
	}

	UreTechEngineClass::c_Instance->mainRenderer = new Renderer();
	UreTechEngineClass::c_Instance->mainRenderer->engine = UreTechEngineClass::c_Instance;
	UreTechEngineClass::c_Instance->mainRenderer->window = UreTechEngineClass::c_Instance->window;

	UreTechEngineClass::c_Instance->mainRenderer->InitVulkan();
	UreTechEngineClass::c_Instance->mainRenderer->InitImGuiVulkan();

	UreTechEngineClass::c_Instance->mainRenderer->createImageSamplers();
	UreTechEngineClass::c_Instance->mainRenderer->VertexShaders.push_back(UreTechEngineClass::c_Instance->mainRenderer->createShaderModule("/shaders/vs.spv"));
	UreTechEngineClass::c_Instance->mainRenderer->FragmentShaders.push_back(UreTechEngineClass::c_Instance->mainRenderer->createShaderModule("/shaders/fs.spv"));
	UreTechEngineClass::c_Instance->mainRenderer->setupShaderUniforms();
	UreTechEngineClass::c_Instance->mainRenderer->createEnginePipeline();

	UreTechEngineClass::c_Instance->defPlayer = new Player;

	// SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	UreTechEngineClass::c_Instance->visualThreading = new multicoreThreading();

	for (int i = 0; i < RENDERER_VULKAN_FENCE_COUNT; i++) {
		UreTechEngineClass::c_Instance->threadCommandBuffers[i].resize(UreTechEngineClass::c_Instance->visualThreading->getThreadCount());
	}

	UreTechEngineClass::c_Instance->threadCommandPools.resize(UreTechEngineClass::c_Instance->visualThreading->getThreadCount());

	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(UreTechEngineClass::c_Instance->mainRenderer->physicalDevice, UreTechEngineClass::c_Instance->mainRenderer->surface);

	for (int i = 0; i < UreTechEngineClass::c_Instance->visualThreading->getThreadCount(); i++) {
		//c_Instance->threadIDs.push_back(i); not used
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		vkCreateCommandPool(UreTechEngineClass::c_Instance->mainRenderer->device, &poolInfo, nullptr, &UreTechEngineClass::c_Instance->threadCommandPools[i]);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = UreTechEngineClass::c_Instance->threadCommandPools[i];
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocInfo.commandBufferCount = 1;

		for (int j = 0; j < RENDERER_VULKAN_FENCE_COUNT; j++) {
			vkAllocateCommandBuffers(UreTechEngineClass::c_Instance->mainRenderer->device, &allocInfo, &UreTechEngineClass::c_Instance->threadCommandBuffers[j][i]);
		}
	}

	// main loop command buffer
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = UreTechEngineClass::c_Instance->mainRenderer->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	allocInfo.commandBufferCount = 1;

	for (int i = 0; i < RENDERER_VULKAN_FENCE_COUNT; i++) {
		vkAllocateCommandBuffers(UreTechEngineClass::c_Instance->mainRenderer->device, &allocInfo, &UreTechEngineClass::c_Instance->mainCommandBuffer[i]);
	}

	UreTechEngineClass::c_Instance->visualThreading->setWorkerFunc(multiThreadVisualWorker);
	UreTechEngineClass::c_Instance->visualThreading->setPreWorkersFunc(multiThreadVisualPreWorkers);
	UreTechEngineClass::c_Instance->visualThreading->setPostWorkersFunc(multiThreadVisualPostWorkers);
	UreTechEngineClass::c_Instance->visualThreading->setPreJobFunc(multiThreadVisualPreJob);
	UreTechEngineClass::c_Instance->visualThreading->setPostJobFunc(multiThreadVisualPostJob);

	return 0;
}

UreTechEngine::UreTechEngineClass* UreTechEngine::UreTechEngineClass::getEngine()
{
	if (c_Instance == nullptr) {
		initEngine();
	}

	return c_Instance;
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

void entity_load_thread(entity* _ent, UreTechEngine::UreTechEngineClass* engRef)
{
	string name = (_ent->entName + " async_load thread");
	std::wstring wname(name.data(), name.data() + name.lenght());
	SetThreadDescription(GetCurrentThread(), wname.c_str());
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	_ent->async_load();

	_ent->status = SUSPENDED;
	_ent->begin();
	_ent->status = READY;
	//UreTechEngine::EngineConsole::log("Entity " + _ent->entName + " loaded!", UreTechEngine::EngineConsole::INFO);
}

UreTechEngine::entity* UreTechEngine::UreTechEngineClass::spawnEntity(entity* _toSpawn)
{
	_toSpawn->entityID = lastID;
	lastID++;
	_toSpawn->init(this);
	std::thread loadThread(entity_load_thread, _toSpawn, this);
	loadThread.detach();

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
	//mainRenderer->setVec4("uLightColor", a);
	stillLoadingEntities = 0;
	for (uint64_t i = 0; i < engRef->getCountOfEntity(); i++) {
		if (engRef->getEntityWithIndex(i)->status == READY) {
			engRef->getEntityWithIndex(i)->tick();
			//engRef->getEntityWithIndex(i)->updateVisual();
		}
		else {
			stillLoadingEntities++;
		}
	}
	
	static timer printTimer(1000);

	if (stillLoadingEntities && printTimer.async_wait()) {
		EngineConsole::log("Still loading " + std::to_string(stillLoadingEntities) + " entities!", EngineConsole::INFO);
		printTimer.reset();
	}

	// run threads if not minimized
	if (!UreTechEngine::UreTechEngineClass::windowMinmized) {
		visualThreading->addJobs(engRef->sceneEntities.data(), engRef->sceneEntities.size(), sizeof(entity*));
		visualThreading->startJobs();
	}
}

void UreTechEngine::UreTechEngineClass::waitEngineTickSync()
{
	/*
	static timer printTimer(1000);
	timer waitTime;
	*/

	visualThreading->waitJobsSync();

	/*
	if (printTimer.async_wait()) {
		EngineConsole::log("Waited " + std::to_string(waitTime.get_elapsed_ms()) + " ms for visual update!", EngineConsole::DEBUG);
		printTimer.reset();
	}
	*/
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

VkCommandBuffer UreTechEngine::UreTechEngineClass::getThreadCommandBuffer()
{
	unsigned int myThreadId = GetThreadId(GetCurrentThread());
	for(size_t i = 0; i < visualThreading->getThreadCount(); i++) {
		if (visualThreading->getThreadID(i) == myThreadId) {
			return threadCommandBuffers[mainRenderer->currentFrameIndex][i];
		}
	}
	return mainCommandBuffer[mainRenderer->currentFrameIndex]; // return main command buffer if not found
}

UreTechEngine::UreTechEngineClass::UreTechEngineClass()
{
}

UreTechEngine::UreTechEngineClass::~UreTechEngineClass()
{
}
