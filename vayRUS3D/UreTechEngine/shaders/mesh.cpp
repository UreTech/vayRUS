#include"mesh.hpp"

#include<../EngineCore.h>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

void mesh::draw(UreTechEngine::Transform3D _addTrnsfm)
{
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
	glm::vec3 TransformTransliton = glm::vec3(transform.Location.fx() + _addTrnsfm.Location.fx(), transform.Location.fy() + _addTrnsfm.Location.fy(), transform.Location.fz() + _addTrnsfm.Location.fz());
	UreTechEngine::Rotation _rot;
	_rot.roll += _addTrnsfm.Rotation.roll;
	_rot.pitch += _addTrnsfm.Rotation.pitch;
	_rot.yaw += _addTrnsfm.Rotation.yaw;
	glm::vec3 TransformRotation = glm::vec3(_rot.fRoll(), _rot.fPitch(), _rot.fYaw());
	glm::mat4 TransformScale = glm::scale(glm::mat4(1), glm::vec3(transform.Scale.fx() * _addTrnsfm.Scale.fx(), transform.Scale.fy() * _addTrnsfm.Scale.fy(), transform.Scale.fz() * _addTrnsfm.Scale.fz()));

	shaderProg->setVec3("uTranslation", TransformTransliton);
	shaderProg->setVec3("uRotation", TransformRotation);
	shaderProg->setMat4("uScale", &TransformScale);

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
