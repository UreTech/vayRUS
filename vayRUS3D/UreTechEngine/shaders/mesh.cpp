#include"mesh.hpp"

#include<../EngineCore.h>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

void mesh::draw(UreTechEngine::Transform3D _addTrnsfm)
{
	if (this == nullptr)return;
	shaderProg->setTexture("texture0", 0);
	shaderProg->setTexture("texture1", 1);
	shaderProg->setTexture("texture2", 2);
	shaderProg->setTexture("texture3", 3);
	shaderProg->setTexture("texture4", 4);
	shaderProg->setTexture("texture5", 5);

	if (useMultipleMaterials) {
		for (int i = 0; i < Materials.size();i++) {
			textures.push_back(Materials[i].colorText);
		}
		textManager->applyMultipleTexture(textures);
	}
	else {
		textManager->applyTexture(GL_TEXTURE0, Materials[0].colorText);
	}
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

	shaderProg->setMat4("uMtxModel", &modelMatrix);



	for (int i = 0; i < Materials.size(); i++) {
		shaderProg->setBool("litRender"+std::to_string(i), Materials[i].litRender);
		shaderProg->setFloat("specularStrength" + std::to_string(i), Materials[i].specularStrength);
	}

	p_Vao->activateBuffer();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	p_Vao->deactivateBuffer();

}

void mesh::applyTexture(texture _text)
{
	TextureID = _text;
}

void mesh::changeLitRender(bool val)
{
	litRender = val;
	shaderProg->setBool("litRender", litRender);
}

mesh::mesh(vertexArrayObject* _p_Vao, Material _mat)
{
	p_Vao = _p_Vao;
	indexCount = p_Vao->getIndexCount();
	this->Materials.push_back(_mat);
	TextureID = 0;
	PlayerRef= UreTechEngine::UreTechEngineClass::getEngine()->getPlayer();
	textManager = TextureManager::getInstance();
	if (textManager == nullptr) {
		UreTechEngine::EngineConsole::log("(Mesh): Failed to use texture system!", UreTechEngine::EngineConsole::ERROR_FATAL);
	}
	shaderProg = UreTechEngine::UreTechEngineClass::getEngine()->getShaderProgram();
}
