#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RIGHT_HANDED
#include<glm/glm.hpp>

#include<UreTechEngine/player/player.h>

void UreTechEngine::Player::updateCamera()
{
	//cam matrix
	glm::vec3 forward;
	forward.x = sin(glm::radians(camRot.yaw)) * cos(glm::radians(camRot.pitch));
	forward.y = sin(glm::radians(camRot.pitch));
	forward.z = -cos(glm::radians(camRot.yaw)) * cos(glm::radians(camRot.pitch));

	forward = glm::normalize(forward);

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
	glm::vec3 up = glm::normalize(glm::cross(right, forward));

	mtxCam = glm::lookAt((glm::vec3)camPos, (glm::vec3)camPos + forward, up);

	//projection matrix
	mtxProj = glm::perspective(glm::radians(camFov), (float(UreTechEngine::UreTechEngineClass::displayWidth) / float(UreTechEngine::UreTechEngineClass::displayHeight)), 0.1f, 1000.0f);

	mtxProj[1][1] *= -1;

	shaderProg->vsUniforms.uMtxProj = this->mtxProj;
	shaderProg->vsUniforms.uMtxCam = this->mtxCam;
	shaderProg->vsUniforms.uPosCam = glm::vec3(camPos.x, camPos.y, camPos.z);
	shaderProg->fsUniforms.lightPos = glm::vec3(camPos.x, camPos.y, camPos.z);
}

UreTechEngine::Player::Player()
{
	engineRef = UreTechEngine::UreTechEngineClass::getEngine();
	shaderProg = engineRef->getShaderProgram();
}
