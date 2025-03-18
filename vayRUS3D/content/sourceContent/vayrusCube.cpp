#include"vayrusCube.h"

#include <../EngineCore.h>

#include<iostream>
vayrusCube::vayrusCube(void) {
	//constructor
	entClassName = "vayrusCube";
}
void vayrusCube::begin() {
	//add your begin commands here
	texture tex = TextureManager::getInstance()->loadTextureFromFile("/test/tText0.jpg");
	Material mymat;
	mymat.colorText = tex;
	this->entityMesh = MeshManager::getInstance()->imp_mesh_obj_type("/test/monke_smth.obj",mymat);
	if (this->entityMesh == nullptr) {
		UreTechEngine::EngineConsole::log("vayRUS cube can not load mesh!", UreTechEngine::EngineConsole::t_error::ERROR_NORMAL);
	}
	entName = "vayrusCube";
	print("vayRUS cube Spawned!");
}

void vayrusCube::tick() {
	//add your loop commands here
}