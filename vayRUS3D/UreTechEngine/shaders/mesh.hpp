#ifndef mesh_hpp
#define mesh_hpp
#include"../UreTechEngine/shaders/shaderSystem.hpp"
#include"../UreTechEngine/shaders/vertexArray.hpp"
#include"../UreTechEngine/shaders/vertexStructs.hpp"
#include"../UreTechEngine/shaders/textureSystem.hpp"
#include"../UreTechEngine/utils/baseVector.h"
#include"../UreTechEngine/player/player.h"
#include"../shaders/Material.h"
#include"../utils/Array.hpp"

#include<vector>


#include<glad/glad.h>
#include <gl/GL.h>
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
	ShaderProgram* shaderProg;
	UreTechEngine::Player* PlayerRef = nullptr;
	unsigned int indexCount;
	texture TextureID;
};


#endif 
