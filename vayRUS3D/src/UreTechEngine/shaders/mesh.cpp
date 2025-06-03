#include"mesh.hpp"

#include<UreTechEngine/EngineCore.h>

#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

void mesh::draw(UreTechEngine::Transform3D _addTrnsfm)
{
	// t*r*s
	modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::rotate(modelMatrix, glm::radians(_addTrnsfm.Rotation.roll + this->transform.Rotation.roll), glm::vec3(1, 0, 0)); // X
	modelMatrix = glm::rotate(modelMatrix, glm::radians(_addTrnsfm.Rotation.pitch + this->transform.Rotation.pitch), glm::vec3(0, 1, 0)); // Y
	modelMatrix = glm::rotate(modelMatrix, glm::radians(_addTrnsfm.Rotation.yaw + this->transform.Rotation.yaw), glm::vec3(0, 0, 1)); // Z

	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
		_addTrnsfm.Location.x + this->transform.Location.x,
		_addTrnsfm.Location.y + this->transform.Location.y,
		_addTrnsfm.Location.z + this->transform.Location.z)) * modelMatrix;

	modelMatrix = glm::scale(modelMatrix, glm::vec3(
		_addTrnsfm.Scale.x * this->transform.Scale.x,
		_addTrnsfm.Scale.y * this->transform.Scale.y,
		_addTrnsfm.Scale.z * this->transform.Scale.z));

	vsUniforms.uMtxModel = modelMatrix;

	/*
	for (int i = 0; i < materialCount; i++) {
		//fix here later!
		fsUniforms.litRender0 = Materials[i].litRender;
		fsUniforms.specularStrength0 = Materials[i].specularStrength;
	}
	*/

	// sending uniforms
	updateUBODescriptor();
	shaderProg->sendUniforms(descriptorSets, 2);

	// binding mesh
	p_Vao->bindBuffer();

}

void mesh::updateTextureDescriptor()
{
	VkWriteDescriptorSet writeTexArray{};
	writeTexArray.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeTexArray.dstSet = textureDescSet;
	writeTexArray.dstBinding = 0;
	writeTexArray.dstArrayElement = 0; 
	writeTexArray.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeTexArray.descriptorCount = 16;
	writeTexArray.pImageInfo = images;

	vkUpdateDescriptorSets(shaderProg->device, 1, &writeTexArray, 0, nullptr);
}

void mesh::updateUBODescriptor()
{
	// update global UBOs
	vsUniforms.uMtxCam = shaderProg->vsUniforms.uMtxCam;
	vsUniforms.uMtxProj = shaderProg->vsUniforms.uMtxProj;
	vsUniforms.uPosCam = shaderProg->vsUniforms.uPosCam;
	fsUniforms.lightPos = shaderProg->fsUniforms.lightPos;
	fsUniforms.uLightColor = shaderProg->fsUniforms.uLightColor;

	void* data;
	vkMapMemory(shaderProg->device, vtxUniformBufMemory, 0, sizeof(baseVS_Uniforms), 0, &data);
	memcpy(data, &vsUniforms, sizeof(baseVS_Uniforms));
	vkUnmapMemory(shaderProg->device, vtxUniformBufMemory);

	vkMapMemory(shaderProg->device, frgUniformBufMemory, 0, sizeof(baseFS_Uniforms), 0, &data);
	memcpy(data, &fsUniforms, sizeof(baseFS_Uniforms));
	vkUnmapMemory(shaderProg->device, frgUniformBufMemory);

	vkUpdateDescriptorSets(shaderProg->device, 1, &vtxWriteUBO, 0, nullptr);
	vkUpdateDescriptorSets(shaderProg->device, 1, &frgWriteUBO, 0, nullptr);
}

void mesh::applyTexture(texture _text)
{
	//deprecated function
}

void mesh::changeLitRender(bool val)
{
	// deprecated function
	litRender = val;
	shaderProg->setBool("litRender", litRender);
}

mesh::mesh(vertexArrayObject* _p_Vao, Material _mat)
{
	modelMatrix = glm::mat4(1.0f);
	p_Vao = _p_Vao;
	shaderProg = UreTechEngine::UreTechEngineClass::getEngine()->getShaderProgram();
	indexCount = p_Vao->getIndexCount();

	for (int i = 0; i < 16; i++) {
		images[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		images[i].imageView = shaderProg->emptyTx.imageView;
		images[i].sampler = shaderProg->emptyTx.imageSampler;
	}

	Materials[0] = _mat;

	PlayerRef= UreTechEngine::UreTechEngineClass::getEngine()->getPlayer();
	textManager = TextureManager::getInstance();
	if (textManager == nullptr) {
		UreTechEngine::EngineConsole::log("(Mesh): Failed to use texture system!", UreTechEngine::EngineConsole::ERROR_FATAL);
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = shaderProg->descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &shaderProg->UTEXdescriptorSetLayout;

	VkDescriptorSetAllocateInfo UBOallocInfo{};
	UBOallocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	UBOallocInfo.descriptorPool = shaderProg->descriptorPool;
	UBOallocInfo.descriptorSetCount = 1;
	UBOallocInfo.pSetLayouts = &shaderProg->UBOdescriptorSetLayout;

	if (vkAllocateDescriptorSets(shaderProg->device, &UBOallocInfo, &UBOdescriptorSet) != VK_SUCCESS) {
		EngineConsole::log("Can not allocate UBOdescriptor set for mesh!", EngineConsole::ERROR_FATAL);
	}

	if (vkAllocateDescriptorSets(shaderProg->device, &allocInfo, &textureDescSet) != VK_SUCCESS) {
		EngineConsole::log("Failed to allocate textureDescSet descriptor sets for mesh!", EngineConsole::ERROR_FATAL);
	}

	updateTextureDescriptor();

	// create uniform buffers
	VkDeviceSize bufferSize = sizeof(baseVS_Uniforms);
	shaderProg->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vtxUniformBuf, vtxUniformBufMemory);

	bufferSize = sizeof(baseFS_Uniforms);
	shaderProg->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		frgUniformBuf, frgUniformBufMemory);

	// vtx ubo info
	vtxBufferInfo.buffer = vtxUniformBuf;
	vtxBufferInfo.offset = 0;
	vtxBufferInfo.range = sizeof(baseVS_Uniforms);

	vtxWriteUBO.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vtxWriteUBO.dstSet = UBOdescriptorSet;
	vtxWriteUBO.dstBinding = 0;
	vtxWriteUBO.dstArrayElement = 0;
	vtxWriteUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vtxWriteUBO.descriptorCount = 1;
	vtxWriteUBO.pBufferInfo = &vtxBufferInfo;

	// frg ubo info
	frgBufferInfo.buffer = frgUniformBuf;
	frgBufferInfo.offset = 0;
	frgBufferInfo.range = sizeof(baseFS_Uniforms);

	frgWriteUBO.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	frgWriteUBO.dstSet = UBOdescriptorSet;
	frgWriteUBO.dstBinding = 1;
	frgWriteUBO.dstArrayElement = 0;
	frgWriteUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	frgWriteUBO.descriptorCount = 1;
	frgWriteUBO.pBufferInfo = &frgBufferInfo;

	descriptorSets[0] = UBOdescriptorSet;
	descriptorSets[1] = textureDescSet;
}

void mesh::assignMaterial(Material _mat, unsigned int index)
{
	Materials[index] = _mat;
	images[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	images[index].imageView = Materials[index].colorText.imageView;
	images[index].sampler = Materials[index].colorText.imageSampler;
	updateTextureDescriptor();
}
