#pragma once
#ifndef baseVector_h
#define baseVector_h

#include<glm/gtc/matrix_transform.hpp>
namespace UreTechEngine {
	struct vector4 {
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 0;
		vector4(float _x = 0.0, float _y = 0.0, float _z = 0.0, float _w = 0.0) {
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
		float fx() {
			return float(x);
		}
		float fy() {
			return float(y);
		}
		float fz() {
			return float(z);
		}
		float fw() {
			return float(w);
		}
	};
	struct vector3 {
		float x = 0;
		float y = 0;
		float z = 0;
		vector3(float _x=0.0f, float _y = 0.0f, float _z = 0.0f) {
			x = _x;
			y = _y;
			z = _z;
		}
		void resetAsInput(vector3 _val) {
			x = _val.x;
			y = _val.y;
			z = _val.z;
		}
		void addX(float _x) {
			x += _x;
		}
		void addY(float _y) {
			y += _y;
		}
		void addZ(float _z) {
			z += _z;
		}
		float fx() {
			return float(x);
		}
		float fy() {
			return float(y);
		}
		float fz() {
			return float(z);
		}
	};
	struct vector2 {
		float x = 0;
		float y = 0;
	};
	struct vector {
		float x = 0;
	};


	struct Rotation
	{
		enum rotationQArea {
			rotERROR=0,
			rotQ0,
			rotQ1,
			rotQ2,
			rotQ3,
		};
		float roll = 0;
		float pitch = 0;
		float yaw = 0;
		Rotation(float _roll = 0, float _pitch = 0, float _yaw = 0) {
			roll = _roll;
			pitch = _pitch;
			yaw = _yaw;
		}
		void resetAsInput(vector3 _val) {
			roll = _val.x;
			pitch = _val.y;
			yaw = _val.z;
		}
		float fRoll() {
			return float(roll);
		}
		float fPitch() {
			return float(pitch);
		}
		float fYaw() {
			return float(yaw);
		}
		glm::mat4 convertRotationMat() {
			glm::mat4 TransformRotation = glm::rotate(glm::mat4(1), glm::radians(float(roll)), glm::vec3(1.0f, 0.0f, 0.0f));
			TransformRotation = glm::rotate(TransformRotation, glm::radians(float(pitch)), glm::vec3(0.0f, 1.0f, 0.0f));
			TransformRotation = glm::rotate(TransformRotation, glm::radians(float(yaw)), glm::vec3(0.0f, 0.0f, 1.0f));
			return TransformRotation;
		}
		glm::mat4 convertRotationMat(glm::mat4& _matr) {
			glm::mat4 TransformRotation = glm::rotate(_matr, glm::radians(float(roll)), glm::vec3(1.0f, 0.0f, 0.0f));
			TransformRotation = glm::rotate(TransformRotation, glm::radians(float(pitch)), glm::vec3(0.0f, 1.0f, 0.0f));
			TransformRotation = glm::rotate(TransformRotation, glm::radians(float(yaw)), glm::vec3(0.0f, 0.0f, 1.0f));
			return TransformRotation;
		}
		Rotation convertMaxFullAngle() {
			int dec_t = 0;
			Rotation result;
			if (roll > 0) {
				dec_t = 0;
				while ((roll-(dec_t*360))>0) {
					dec_t++;
				}
				dec_t--;
				result.roll = roll - (dec_t * 360);
			}
			else {
				dec_t = 0;
				while ((roll - (dec_t * -360)) < 0) {
					dec_t++;
				}
				dec_t--;
				result.roll = 360 + (roll - (dec_t * -360));
			}

			if (pitch > 0) {
				dec_t = 0;
				while ((pitch - (dec_t * 360)) > 0) {
					dec_t++;
				}
				dec_t--;
				result.pitch = pitch - (dec_t * 360);
			}
			else {
				dec_t = 0;
				while ((pitch - (dec_t * -360)) < 0) {
					dec_t++;
				}
				dec_t--;
				result.pitch = 360 + (pitch - (dec_t * -360));
			}

			if (yaw > 0) {
				dec_t = 0;
				while ((yaw - (dec_t * 360)) > 0) {
					dec_t++;
				}
				dec_t--;
				result.yaw = yaw - (dec_t * 360);
			}
			else {
				dec_t = 0;
				while ((yaw - (dec_t * -360)) < 0) {
					dec_t++;
				}
				dec_t--;
				result.yaw = 360 + (yaw - (dec_t * -360));
			}
			return result;
		}

		rotationQArea getQAreaRoll() {
			Rotation t_rot = this->convertMaxFullAngle();
			if (t_rot.roll >= 0 && t_rot.roll <= 90) {
				return rotQ0;
			}else if (t_rot.roll > 90 && t_rot.roll <= 180) {
				return rotQ1;
			}else if (t_rot.roll > 180 && t_rot.roll <= 270) {
				return rotQ2;
			}else if (t_rot.roll > 270 && t_rot.roll <= 360) {
				return rotQ3;
			}
			else {
				return rotERROR;
			}
		}
		rotationQArea getQAreaPitch() {
			Rotation t_rot = this->convertMaxFullAngle();
			if (t_rot.pitch >= 0 && t_rot.pitch <= 90) {
				return rotQ0;
			}
			else if (t_rot.pitch > 90 && t_rot.pitch <= 180) {
				return rotQ1;
			}
			else if (t_rot.pitch > 180 && t_rot.pitch <= 270) {
				return rotQ2;
			}
			else if (t_rot.pitch > 270 && t_rot.pitch <= 360) {
				return rotQ3;
			}
			else {
				return rotERROR;
			}
		}
		rotationQArea getQAreaYaw() {
			Rotation t_rot = this->convertMaxFullAngle();
			if (t_rot.yaw >= 0 && t_rot.yaw <= 90) {
				return rotQ0;
			}
			else if (t_rot.yaw > 90 && t_rot.yaw <= 180) {
				return rotQ1;
			}
			else if (t_rot.yaw > 180 && t_rot.yaw <= 270) {
				return rotQ2;
			}
			else if (t_rot.yaw > 270 && t_rot.yaw <= 360) {
				return rotQ3;
			}
			else {
				return rotERROR;
			}
		}

	};
	typedef Rotation rot;
	struct Transform3D
	{
		vector3 Location;
		rot Rotation;
		vector3 Scale= vector3(1.0f, 1.0f, 1.0f);

		Transform3D(vector3& _loc, rot& _rot,vector3& _scl) {
			Location = _loc;
			Rotation = _rot;
			Scale = _scl;
		}
		Transform3D() {}

		Transform3D(const vector3& Location, const rot& Rotation, const vector3& Scale)
			: Location(Location), Rotation(Rotation), Scale(Scale)
		{
		}
	};

}

#endif 