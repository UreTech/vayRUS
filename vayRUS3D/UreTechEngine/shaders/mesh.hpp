#ifndef mesh_hpp
#define mesh_hpp
#include<../EngineCore.h>
#include<../UreTechEngine/shaders/Material.h>
#include<vector>

#include<GLFW/glfw3.h>

namespace UreTechEngine {
	class Player;
}
class TextureManager;
typedef unsigned int texture;
class mesh {
public:
	std::vector<Material> Materials;
	bool useMultipleMaterials = false;

	std::vector<texture> textures;

	UreTechEngine::Transform3D transform;
	void draw(UreTechEngine::Transform3D _addTrnsfm);
	void applyTexture(texture _text);
	void changeLitRender(bool val);
	mesh(vertexArrayObject* _p_Vao, Material _mat);
private:
	bool litRender = true;
	vertexArrayObject* p_Vao;
	TextureManager* textManager;
	Renderer* shaderProg;
	glm::mat4 modelMatrix;
	UreTechEngine::Player* PlayerRef = nullptr;
	unsigned int indexCount;
	texture TextureID;
};


#endif 
