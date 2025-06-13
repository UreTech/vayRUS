#pragma once
#ifndef MyPlayerPawn_h
#define MyPlayerPawn_h
#include<UreTechEngine/entity/entity.h>

using namespace UreTechEngine;
class MyPlayerPawn : public UreTechEngine::entity{
public:
	MyPlayerPawn(void);
	MyPlayerPawn(mesh* _mesh, UreTechEngine::string _entName, UreTechEngine::Transform3D trnsfrm = Transform3D(vector3(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f)));
	void async_load(VkCommandBuffer cmdBuffer);
	void begin();
	void tick();
private:
};

// added to register
ENGINE_ENTITY_REGISTER("UreTechEngine", "MyPlayerPawn", MyPlayerPawn, UreTechEngine_MyPlayerPawn_Index);

#endif // !MyPlayerPawn_h

