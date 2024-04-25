#pragma once
#ifndef entity_h
#define entity_h

#include <string>
#include"../UreTechEngine/utils/baseVector.h"
#include"../UreTechEngine/shaders/mesh.hpp"

class mesh;

namespace UreTechEngine {
	class UreTechEngineClass;
	class entity {
	public:
		std::string entName = "NONE";
		std::string entClassName = "entity";
		std::string entCustomSets = "null";
		UreTechEngine::Transform3D transform;
		mesh* entityMesh;
		UreTechEngineClass* engineRef=nullptr;
		void updateVisual();
		void init(UreTechEngineClass* _ptr);
		virtual void begin();
		virtual void tick();
		entity() = default;
		entity(UreTechEngine::vector3& location, UreTechEngine::Rotation& rot, UreTechEngine::vector3& scale, mesh* _mesh, std::string _entName);
		entity(UreTechEngine::vector3& location, mesh* _mesh, std::string _entName);
		entity(mesh* _mesh, std::string _entName, UreTechEngine::Transform3D trnsfrm = Transform3D(vector3(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f)));
	};
}
#endif 