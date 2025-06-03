#include<UreTechEngine/EngineCore.h>

void UreTechEngine::Player::updateCamera()
{
	//cam matrix
	mtxCam =
		glm::rotate(glm::mat4(1.0f), glm::radians(camRot.yaw), glm::vec3(0, 0, 1)) *//z
		glm::rotate(glm::mat4(1.0f), glm::radians(camRot.pitch), glm::vec3(0, 1, 0)) *//y
		glm::rotate(glm::mat4(1.0f), glm::radians(camRot.roll), glm::vec3(1, 0, 0));//x
	mtxCam = glm::transpose(mtxCam);
	mtxCam = glm::translate(mtxCam, -glm::vec3(camPos.x, camPos.y, camPos.z));

	//projection matrix
	mtxProj = glm::perspective(glm::radians(camFov), (float(UreTechEngine::UreTechEngineClass::displayWidth) / float(UreTechEngine::UreTechEngineClass::displayHeight)), 1.0f, 1000.0f);

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
