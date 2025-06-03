#ifndef textureSystem_hpp
#define textureSystem_hpp

#include<string>
#include<map>
#include<vector>
#include<UreTechEngine/EngineCore.h>

namespace UreTechEngine {
	class UreTechEngineClass;
}

struct loadedTexture {
	UreTechEngine::string texuteLoadName = "NO NAME";
	texture texture;
};

class TextureManager{

public:
	static TextureManager* getInstance();

	texture loadTextureFromFile(UreTechEngine::string fileName, bool texAntiAlising = true);
	void applyTexture(int levelInd, texture tex);
	void applyMultipleTexture(std::vector<texture> a);

private:
	Renderer* shaderProg;
	UreTechEngine::UreTechEngineClass* engine;
	TextureManager();
	static TextureManager* c_Instance;
	UreTechEngine::dArray<loadedTexture> loadedTexts;
};



#endif