#pragma once
#ifndef vayrusCube_h
#define vayrusCube_h

#include<UreTechEngine/entity/entity.h>

class vayrusCube :public UreTechEngine::entity {
public:
	vayrusCube(void);
	void async_load(VkCommandBuffer cmdBuffer);
	void begin();
	void tick();
};
// added to register
ENGINE_ENTITY_REGISTER("UreTechEngine", "vayrusCube", vayrusCube, UreTechEngine_vayrusCube_Index);
#endif

