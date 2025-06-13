#pragma once
#include<UreTechEngine/utils/multiThreadWorker.h>
#include<UreTechEngine/EngineCore.h>

#include <mutex>
#include <thread>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"nlohmann/json.hpp"

#include<UreTechEngine/upk/upk.h>

//class multicoreThreading;
#include<UreTechEngine/gmodule/gmodule.h>

class mesh;
class Material;
class upk_API;

namespace UreTechEngine {
	class entity;
	class Player;

	typedef entity* (*entConstructFunc)();

	struct ENGINE_DEFINE entConstructStruct {
		string entClassName;
		string entClassNamespace;
		entConstructFunc constructor = nullptr;
		
		entConstructStruct() {
			entClassName = "ERROR CLASS TYPE";
			entClassNamespace = "ERROR NAMESPACE";
			constructor = nullptr;
		}

		entConstructStruct(UreTechEngine::string _entClassName , entConstructFunc _constructor, UreTechEngine::string _entClassNamespace) {
			entClassName = _entClassName;
			entClassNamespace = _entClassNamespace;
			constructor = _constructor;
		}
	};


	class ENGINE_DEFINE UreTechEngineClass {
	friend class Renderer;
	public:
		static unsigned int displayWidth;
		static unsigned int displayHeight;
		static  bool windowMinmized;
		static bool externalConsoleState;
		bool isInServer = false;
		bool isServer = true;

		upk_API* package = nullptr;

		bool useUpkSystem = false;

		//std::map<std::string, Material> loadedMaterials;
		//std::map<std::string, texture> loadedTextures;

		static dArray<entConstructStruct> entityConstructors;

		Renderer* mainRenderer = nullptr;
		static UreTechEngineClass* c_Instance;
		static UreTechEngineClass* getEngine();
		void setKeyCallBackFunc(GLFWkeyfun func, GLFWmousebuttonfun funcM);
		Player* getPlayer();
		Renderer* getShaderProgram();
		GLFWwindow* getWindow();
		entity* spawnEntity(entity* _toSpawn);
		entity* getEntityWithIndex(unsigned int _index);
		entity* getEntityWithName(UreTechEngine::string _entName);
		entity* getEntityWithID(uint64_t id);
		bool isThisEntNameAvilable(UreTechEngine::string _entName);
		bool isValidEntity(uint64_t id);
		unsigned int getCountOfEntity();

		void engineTick();

		bool killEntity(entity* _ent);
		bool killEntity(string entName);

		void init_upk_system(string path, string encKey = "NOKEY");
		upk_API* imp_upk_package(string path, string encKey = "NOKEY");

		void loadMap(std::string mapPath);
		void saveCurrentMap(std::string mapPath);
		void saveGame(std::string gamePath);
		void loadGame(std::string gamePath);

	private:
		// visual update
		multicoreThreading visualThreading;
		dArray<VkCommandBuffer> threadCommandBuffers;
		dArray<VkCommandPool> threadCommandPools;

		size_t systemThreadCount = std::thread::hardware_concurrency();
		nlohmann::json mapJson;
		Player* defPlayer = nullptr;
		GLFWwindow* window = nullptr;
		uint64_t lastID = 0x000a0000; // BEGIN_ENT_ID define
		dArray<entity*> sceneEntities;
		dArray<upk_API*> loadedPackages;
		size_t stillLoadingEntities = 0;
		unsigned int netPlayersCount = 0;
		Player* netPlayers[5];
		UreTechEngineClass();
		~UreTechEngineClass();
	};
}