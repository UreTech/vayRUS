#ifndef player_h
#define player_h

#include"../UreTechEngine/utils/baseVector.h"
#include"../UreTechEngine/shaders/Renderer.hpp"
#include"../entity/entity.h"

class Renderer;
namespace UreTechEngine {
	class UreTechEngineClass;
	class entity;
	class Player {
	public:
		vector3 camPos = 0;
		Rotation camRot = 0;
		float camFov = 90.0f;
		glm::mat4 mtxCam = glm::mat4();
		glm::mat4 mtxProj = glm::mat4();

		Renderer* shaderProg;
		UreTechEngineClass* engineRef=nullptr;

		UreTechEngine::Transform3D CameraTranform;
		entity* playerPawn = nullptr;
		void updateCamera();
		Player();

	private:
	};
}
#endif
