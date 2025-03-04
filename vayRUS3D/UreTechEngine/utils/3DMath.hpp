#pragma once
#include"baseVector.h"
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

namespace UreTechEngine {
	class Math3D{
	public:
		static vector3 rotateAround(vector3 point,vector3 pivot, Rotation rotation) {
			Rotation convRot = rotation;
			
			glm::mat3 rotX(
				1, 0, 0,
				0, cos(glm::radians(rotation.roll)), -sin(glm::radians(rotation.roll)),
				0, sin(glm::radians(rotation.roll)), cos(glm::radians(rotation.roll))
			);

			glm::mat3 rotY(
				cos(glm::radians(rotation.pitch)), 0, sin(glm::radians(rotation.pitch)),
				0, 1, 0,
				-sin(glm::radians(rotation.pitch)), 0, cos(glm::radians(rotation.pitch))
			);

			glm::mat3 rotZ(
				cos(glm::radians(rotation.yaw)), -sin(glm::radians(rotation.yaw)), 0,
				sin(glm::radians(rotation.yaw)), cos(glm::radians(rotation.yaw)), 0,
				0, 0, 1
			);

			glm::vec3 trnslt = glm::vec3(point.x, point.y, point.z) - glm::vec3(pivot.x, pivot.y, pivot.z);
			glm::vec3 reslt = (glm::inverse(rotX * rotY * rotZ) * trnslt) + glm::vec3(pivot.x, pivot.y, pivot.z);
			return vector3(reslt.x, reslt.y, reslt.z);
		}

		static vector3 addWithRotation(Rotation _rot,vector3 _force,vector3 _toAdd) {
			vector3 result = _toAdd;

			Rotation absRot = _rot.convertMaxFullAngle();

			double radians = (absRot.yaw-90) * 3.14159f / 180.0f;

			double exactXxChange = _force.x *-1* cos(radians);
			double exactXyChange = _force.x *-1* sin(radians);

			double exactYxChange= _force.y *-1* sin(radians);
			double exactYyChange = _force.y *-1* cos(radians);

			result.x = exactXxChange+ exactYxChange +_toAdd.x;
			result.y = exactXyChange- exactYyChange + _toAdd.y;

			return result;
		}
	};
}