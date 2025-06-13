#include "MyPlayerPawn.h"
#include<UreTechEngine/EngineCore.h>

MyPlayerPawn::MyPlayerPawn(void)
{
	entClassName = "MyPlayerPawn";
}

MyPlayerPawn::MyPlayerPawn(mesh* _mesh, UreTechEngine::string _entName, UreTechEngine::Transform3D trnsfrm)
{
	transform = trnsfrm;
	entityMesh = _mesh;
	entName = _entName;
}

void MyPlayerPawn::async_load(VkCommandBuffer cmdBuffer) {
	texture text = TextureManager::getInstance()->loadTextureFromFile("/test/tText1.jpg", cmdBuffer);
	Material myMat;
	myMat.colorText = text;
	this->entityMesh = MeshManager::getInstance()->imp_mesh_obj_type("/test/cube.obj", myMat);
	UreTechEngine::EngineConsole::log("oldu brom!", UreTechEngine::EngineConsole::INFO);
}

void MyPlayerPawn::begin()
{

}

void MyPlayerPawn::tick()
{
	engineRef->getPlayer()->camPos = this->transform.Location;
}
