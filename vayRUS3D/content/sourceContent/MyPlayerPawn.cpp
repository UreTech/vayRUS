#include "MyPlayerPawn.h"
#include<../EngineCore.h>

MyPlayerPawn::MyPlayerPawn(void)
{
}

MyPlayerPawn::MyPlayerPawn(mesh* _mesh, std::string _entName, UreTechEngine::Transform3D trnsfrm)
{
	transform = trnsfrm;
	entityMesh = _mesh;
	entName = _entName;
}

void MyPlayerPawn::begin()
{
	texture text = TextureManager::getInstance()->loadTextureFromFile("content/Textures/susTM.png");
	Material myMat;
	myMat.colorText = text;
	this->entityMesh = MeshManager::getInstance()->importMeshFbx("content/Meshs/defaultCapsule.obj", myMat);
	UreTechEngine::EngineConsole::consoleError("oldu brom!", UreTechEngine::EngineConsole::INFO_NORMAL);
}
