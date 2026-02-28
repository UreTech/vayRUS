#ifndef textureSystem_hpp
#define textureSystem_hpp

#include<string>
#include<map>
#include<vector>
#include<UreTechEngine/EngineCore.h>
#include<UreTechEngine/utils/dArray.h>

namespace UreTechEngine {
	class UreTechEngineClass;
}

struct loadedTexture {
	UreTechEngine::string textureLoadName = "NO NAME";
	texture* texture = nullptr;
	bool filteredTexture = false;
};

struct inQueueTexture {
	loadedTexture toLoad;
	UreTechEngine::dArray<bool*> waitingFlags;
};

class ENGINE_DEFINE TextureManager{

public:
	static TextureManager* getInstance();

	texture* loadTextureFromFile(UreTechEngine::string fileName, bool filteredTexture = true);

private:
	Renderer* shaderProg;
	UreTechEngine::UreTechEngineClass* engine;
	TextureManager();
	static TextureManager* c_Instance;
	UreTechEngine::dArray<loadedTexture> loadedTexts;

	UreTechEngine::dArray<inQueueTexture> queue;
	std::mutex queueMutex;
};

#endif