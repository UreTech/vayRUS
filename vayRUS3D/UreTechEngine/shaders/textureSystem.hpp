#ifndef textureSystem_hpp
#define textureSystem_hpp

#include<string>
#include<map>
#include<vector>
#include<../EngineCore.h>

typedef unsigned int texture;

namespace UreTechEngine {
	class UreTechEngineClass;
}

struct loadedTexture {
	UreTechEngine::string texuteLoadName = "NO NAME";
	texture texId = -1;
};

class TextureManager{

public:
	static TextureManager* getInstance();

	unsigned int loadTextureFromFile(UreTechEngine::string fileName, bool texAntiAlising = true);
	void applyTexture( int levelInd, unsigned int textureId);
	void applyMultipleTexture(std::vector<texture> a);

private:
	ShaderProgram* shaderProg;
	UreTechEngine::UreTechEngineClass* engine;
	TextureManager();
	static TextureManager* c_Instance;
	UreTechEngine::dArray<loadedTexture> loadedTexts;
};



#endif