#pragma once
#ifndef enginebase_h
#define enginebase_h

#include <../EngineCore.h>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"nlohmann/json.hpp"

#include<../UreTechEngine/upk/upk.h>

class mesh;
class Material;
class upk_API;

namespace UreTechEngine {
	class entity;
	class Player;

	typedef entity* (*entConstructFunc)();

	struct entConstructStruct {
		string entClassName;
		entConstructFunc constructor = nullptr;
		entConstructStruct() {
			entClassName = "ERROR CLASS TYPE";
			constructor = nullptr;
		}
		entConstructStruct(std::string _entClassName , entConstructFunc _constructor ) {
			entClassName = _entClassName;
			constructor = _constructor;
		}
	};

	class UreTechEngineClass {
	typedef unsigned char texture;
	public:
		static unsigned int displayWidth;
		static unsigned int displayHeight;
		static bool windowMinmized;
		bool isInServer = false;
		bool isServer = true;

		upk_API* package = nullptr;

		bool useUpkSystem = false;

		std::map<std::string, Material> loadedMaterials;
		std::map<std::string, texture> loadedTextures;

		dArray<entConstructStruct> entityConstructors;

		ShaderProgram* mainShaderProgram = nullptr;
		static UreTechEngineClass* c_Instance;
		static UreTechEngineClass* getEngine();
		void setKeyCallBackFunc(GLFWkeyfun func, GLFWmousebuttonfun funcM);
		Player* getPlayer();
		ShaderProgram* getShaderProgram();
		GLFWwindow* getWindow();
		entity* spawnEntity(entity* _toSpawn);
		entity* getEntityWithIndex(unsigned int _index);
		entity* getEntityWithName(std::string _entName);
		entity* getEntityWithID(uint64_t id);
		bool isThisEntNameAvilable(std::string _entName);
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
		nlohmann::json mapJson;
		Player* defPlayer = nullptr;
		GLFWwindow* window = nullptr;
		uint64_t lastID = 0x000a0000; // BEGIN_ENT_ID define
		dArray<entity*> sceneEntities;
		dArray<upk_API*> loadedPackages;
		unsigned int netPlayersCount = 0;
		Player* netPlayers[5];
		UreTechEngineClass();
		~UreTechEngineClass();
	};
}
#endif // !enginebase_h