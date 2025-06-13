#ifndef mesh_hpp
#define mesh_hpp
#include<UreTechEngine/EngineCore.h>
#include<UreTechEngine/shaders/Material.h>
#include<vector>

#include<GLFW/glfw3.h>

class vertexArrayObject;

namespace UreTechEngine {
	class Player;
}
class TextureManager;
class Material;

class ENGINE_DEFINE mesh {
public:
	UreTechEngine::Transform3D transform;
	void draw(UreTechEngine::Transform3D _addTrnsfm);
	void changeLitRender(bool val);
	mesh(vertexArrayObject* _p_Vao, Material _mat);

	void assignMaterial(Material _mat, unsigned int index);
private:
	VkDescriptorImageInfo images[16];
	Material Materials[16];

	bool litRender = true;
	vertexArrayObject* p_Vao;
	TextureManager* textManager;
	Renderer* shaderProg;
	glm::mat4 modelMatrix;
	UreTechEngine::Player* PlayerRef = nullptr;
	unsigned int indexCount;

	// uniforms
	baseVS_Uniforms vsUniforms;
	baseFS_Uniforms fsUniforms;

	// new texture update (descriptor set)
	VkDescriptorSet textureDescSet;

	void updateTextureDescriptor();

	VkBuffer vtxUniformBuf;
	VkBuffer frgUniformBuf;
	VkDeviceMemory vtxUniformBufMemory;
	VkDeviceMemory frgUniformBufMemory;
	VkDescriptorSet UBOdescriptorSet;

	VkDescriptorBufferInfo vtxBufferInfo{};
	VkDescriptorBufferInfo frgBufferInfo{};
	VkWriteDescriptorSet vtxWriteUBO{};
	VkWriteDescriptorSet frgWriteUBO{};

	VkDescriptorSet descriptorSets[2];

	void updateUBODescriptor();

	//texture Texture;
	// not usable
	void applyTexture(texture _text);
};


#endif 
