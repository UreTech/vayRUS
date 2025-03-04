#include"vayrusCube.h"

#include <../EngineCore.h>

#include<iostream>
vayrusCube::vayrusCube(void) {
	//constructor
	entClassName = "vayrusCube";
}
void vayrusCube::begin() {
	//add your begin commands here
	texture tex = TextureManager::getInstance()->loadTextureFromFile("textures/text2.png");
	Material mymat;
	mymat.colorText = tex;
	entityMesh = MeshManager::getInstance()->importMeshFbx("a.obj",mymat);
	entName = "vayrusCube";
	UreTechEngine::EngineConsole::log("vayRUS cube Spawned!", UreTechEngine::EngineConsole::t_error::DEBUG);
}

void vayrusCube::tick() {
	//add your loop commands here
}