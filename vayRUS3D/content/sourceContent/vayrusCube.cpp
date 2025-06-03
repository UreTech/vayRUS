#include"vayrusCube.h"

#include<UreTechEngine/EngineCore.h>

#include<iostream>
vayrusCube::vayrusCube(void) {
	//constructor
	entClassName = "vayrusCube";
}
void vayrusCube::begin() {
	//add your begin commands here
	Material nullmat;

	this->entityMesh = MeshManager::getInstance()->imp_mesh_obj_type("/test/gordonHL2.obj", nullmat);

	Material suit;
	suit.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_suit.png");
	this->entityMesh->assignMaterial(suit, 0);

	Material glass;
	glass.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/combine_health_charger_glass.png");
	this->entityMesh->assignMaterial(glass, 1);

	Material skin;
	skin.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/combine_health_charger_glass.png");
	this->entityMesh->assignMaterial(skin, 2);

	Material eye;
	eye.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_eye.png");
	this->entityMesh->assignMaterial(eye, 3);

	Material hair;
	hair.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_hair.png");
	this->entityMesh->assignMaterial(hair, 4);

	Material haircap;
	haircap.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_haircap.png");
	this->entityMesh->assignMaterial(haircap, 5);

	Material head;
	head.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_head.png");
	this->entityMesh->assignMaterial(head, 6);

	Material eyemen;
	eyemen.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eyemeniscus.png");
	this->entityMesh->assignMaterial(eyemen, 7);

	Material eyelas;
	eyelas.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_eye_lash.png");
	this->entityMesh->assignMaterial(eyelas, 8);

	Material eyesha;
	eyesha.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eyeshadow.png");
	this->entityMesh->assignMaterial(eyesha, 9);

	Material brow;
	brow.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_brow.png");
	this->entityMesh->assignMaterial(brow, 10);

	Material brow2;
	brow2.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_brow2.png");
	this->entityMesh->assignMaterial(brow2, 11);

	Material beard;
	beard.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_brow2.png");
	this->entityMesh->assignMaterial(beard, 12);

	Material teeth;
	teeth.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gman_teeth.png");
	this->entityMesh->assignMaterial(teeth, 13);

	Material tong;
	tong.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gman_teeth.png");
	this->entityMesh->assignMaterial(tong, 14);

	Material armor;
	armor.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_suit.png");
	this->entityMesh->assignMaterial(armor, 15);

	if (this->entityMesh == nullptr) {
		UreTechEngine::EngineConsole::log("vayRUS cube can not load mesh!", UreTechEngine::EngineConsole::t_error::LOG_ERROR);
	}
	entName = "vayrusCube";
	print("vayRUS cube Spawned!");
}

void vayrusCube::tick() {
	//add your loop commands here
	this->transform.Rotation.pitch += 0.5f;
}