#include<../EngineCore.h>

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

	shaderProg->setMat4("uMtxProj", &this->mtxProj);
	shaderProg->setMat4("uMtxCam", &this->mtxCam);
	shaderProg->setVec3("uPosCam", glm::vec3(camPos.x, camPos.y, camPos.z));
	shaderProg->setVec3("lightPos", glm::vec3(1000.0f, 1000.0f, 1000.0f));
}

UreTechEngine::Player::Player()
{
	engineRef = UreTechEngine::UreTechEngineClass::getEngine();
	shaderProg = engineRef->getShaderProgram();
}
