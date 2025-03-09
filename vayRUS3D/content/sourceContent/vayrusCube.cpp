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
	entityMesh = MeshManager::getInstance()->imp_mesh_obj_type("a.obj",mymat);
	if (entityMesh == nullptr) {
		UreTechEngine::EngineConsole::log("vayRUS cube can not load mesh!", UreTechEngine::EngineConsole::t_error::ERROR_NORMAL);
	}
	entName = "vayrusCube";
	print("vayRUS cube Spawned!");
}

void vayrusCube::tick() {
	//add your loop commands here
}