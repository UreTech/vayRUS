#include<../EngineCore.h>

void UreTechEngine::Player::updateCamera()
{
	glm::vec3 camPos;
	Rotation camRot;
	if (playerPawn != nullptr) {
		camPos = glm::vec3(CameraTranform.Location.fx() + playerPawn->transform.Location.x, CameraTranform.Location.fy() + playerPawn->transform.Location.y, CameraTranform.Location.fz() + playerPawn->transform.Location.z);
		vector3 point(camPos.x, camPos.y, camPos.z);
		vector3 finPos = Math3D::rotateAround(point, playerPawn->transform.Location, CameraTranform.Rotation);
		camPos = glm::vec3(finPos.x, finPos.y, finPos.z);
		camRot = Rotation(CameraTranform.Rotation.roll, CameraTranform.Rotation.pitch, CameraTranform.Rotation.yaw);
	}
	else {
		camPos = glm::vec3(CameraTranform.Location.fx(), CameraTranform.Location.fy(), CameraTranform.Location.fz());
		camRot = CameraTranform.Rotation;
	}

	mtxCamPos = glm::translate(glm::mat4(1),camPos);
	CamRot = glm::vec3(camRot.fRoll(), camRot.fYaw(), camRot.fPitch());//ilk bu çarp
	mtxCam = glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1.0f, 0.0001f, 0.0001f));

	mtxProj = glm::perspective(glm::radians(90.0f), (float(UreTechEngine::UreTechEngineClass::displayWidth) / float(UreTechEngine::UreTechEngineClass::displayHeight)), 1.0f, 1000.0f);

	shaderProg->setMat4("uMtxProj", &this->mtxProj);
	shaderProg->setVec3("uCamRot", this->CamRot);
	shaderProg->setMat4("uMtxCamPos", &this->mtxCamPos);
	shaderProg->setMat4("uMtxCam", &this->mtxCam);
	shaderProg->setVec3("lightPos", glm::vec3(1000.0f, 1000.0f, 1000.0f));
}

UreTechEngine::Player::Player()
{
	engineRef = UreTechEngine::UreTechEngineClass::getEngine();
	shaderProg = engineRef->getShaderProgram();
}
