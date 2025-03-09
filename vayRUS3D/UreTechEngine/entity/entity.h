#pragma once
#ifndef entity_h
#define entity_h

#include <string>
#include <../EngineCore.h>

class mesh;

#define NULL_ENT_ID  0xffffffff
#define BEGIN_ENT_ID 0x000a0000 // engine doesn't use that
typedef uint64_t entID;

namespace UreTechEngine {
	class UreTechEngineClass;
	class entity {
	public:
		string entName = "NULL ENTITY";
		string entClassName = "UNDEFINED ENTITY CLASS";
		string entCustomSets = "null";
		entID entityID = NULL_ENT_ID;
		UreTechEngine::Transform3D transform;
		mesh* entityMesh;
		UreTechEngineClass* engineRef=nullptr;
		void updateVisual();
		void init(UreTechEngineClass* _ptr);
		void print(string message);
		virtual void begin();
		virtual void tick();
		entity();
		entity(UreTechEngine::vector3& location, UreTechEngine::Rotation& rot, UreTechEngine::vector3& scale, mesh* _mesh, std::string _entName);
		entity(UreTechEngine::vector3& location, mesh* _mesh, std::string _entName);
		entity(mesh* _mesh, std::string _entName, UreTechEngine::Transform3D trnsfrm = Transform3D(vector3(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f)));
	};
}
#endif 