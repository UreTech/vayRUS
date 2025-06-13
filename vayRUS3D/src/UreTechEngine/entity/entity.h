#pragma once
#include<UreTechEngine/EngineCore.h>
#include<UreTechEngine/utils/EngineRegisters.h>

class mesh;

#define NULL_ENT_ID  0xffffffff
#define BEGIN_ENT_ID 0x000a0000 // engine doesn't use that
typedef uint64_t entID;

#define ENGINE_ENTITY_REGISTER(CLASS_NAMESPACE, ENT_CLASS_NAME, CLASS, INDEX_VAR_NAME) \
	static uint64_t INDEX_VAR_NAME = []() { return uEngine_reg_addEntity(ENT_CLASS_NAME, []() { return dynamic_cast<UreTechEngine::entity*>(new CLASS()); }, CLASS_NAMESPACE); }();

#define ENGINE_GET_REGISTERED_ENTITY(CLASS_NAMESPACE, ENT_CLASS_NAME) \
	uEngine_reg_getEntityWithName(CLASS_NAMESPACE, ENT_CLASS_NAME)

namespace UreTechEngine {
	class UreTechEngineClass;

	typedef enum ent_status{
		WAITING_LOAD    = 0x00FA,
		STILL_ON_LOAD   = 0x00FB,
		READY           = 0x00FC,
		PENDING_DESTROY = 0x00FD,
		SUSPENDED       = 0x00FE,
	};

	class ENGINE_DEFINE entity {
	public:
		string entName = "NULL ENTITY";
		string entClassName = "UNDEFINED ENTITY CLASS";
		string entCustomSets = "null";
		ent_status status = WAITING_LOAD;
		entID entityID = NULL_ENT_ID;
		UreTechEngine::Transform3D transform = Transform3D();
		mesh* entityMesh = nullptr;
		UreTechEngineClass* engineRef = nullptr;
		void updateVisual();
		void init(UreTechEngineClass* _ptr);
		void print(string message);
		virtual void async_load(VkCommandBuffer cmdBuffer);
		virtual void begin();
		virtual void tick();
		entity();
		entity(UreTechEngine::vector3& location, UreTechEngine::Rotation& rot, UreTechEngine::vector3& scale, mesh* _mesh, UreTechEngine::string _entName);
		entity(UreTechEngine::vector3& location, mesh* _mesh, UreTechEngine::string _entName);
		entity(mesh* _mesh, UreTechEngine::string _entName, UreTechEngine::Transform3D trnsfrm = Transform3D(vector3(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f)));
	};
}
// added to register
ENGINE_ENTITY_REGISTER("UreTechEngine", "entity", UreTechEngine::entity, UreTechEngine_entity_Index);