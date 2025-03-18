#include "MyPlayerPawn.h"
#include<../EngineCore.h>

MyPlayerPawn::MyPlayerPawn(void)
{
	entClassName = "MyPlayerPawn";
}

MyPlayerPawn::MyPlayerPawn(mesh* _mesh, std::string _entName, UreTechEngine::Transform3D trnsfrm)
{
	transform = trnsfrm;
	entityMesh = _mesh;
	entName = _entName;
}

void MyPlayerPawn::begin()
{
	texture text = TextureManager::getInstance()->loadTextureFromFile("/test/tText0.jpg");
	Material myMat;
	myMat.colorText = text;
	this->entityMesh = MeshManager::getInstance()->imp_mesh_obj_type("/test/monke.obj", myMat);
	UreTechEngine::EngineConsole::log("oldu brom!", UreTechEngine::EngineConsole::INFO_NORMAL);
}
