#include"vayrusCube.h"

#include<UreTechEngine/EngineCore.h>

#include<iostream>
vayrusCube::vayrusCube(void) {
	//constructor
	entClassName = "vayrusCube";
}

void vayrusCube::async_load() {
	// add your async load commands here like loading meshes, textures, etc.
	Material nullmat;

	this->entityMesh = MeshManager::getInstance()->imp_mesh_glb_type("/test/gordonHL2.glb");

	Material suit;
	suit.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_suit.png");
	this->entityMesh->assignMaterial(suit, 0);

	Material glasses;
	glasses.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/combine_health_charger_glass.png");
	this->entityMesh->assignMaterial(glasses, 1);

	Material glass_frame;
	glass_frame.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_eye_lash.png");
	this->entityMesh->assignMaterial(glass_frame, 2);

	Material eye;
	eye.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/Eye-Green.jpeg");
	this->entityMesh->assignMaterial(eye, 3);

	Material hair;
	hair.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_hair.png");
	this->entityMesh->assignMaterial(hair, 4);

	Material hair_cap;
	hair_cap.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_haircap.png");
	this->entityMesh->assignMaterial(hair_cap, 5);

	Material head;
	head.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gordon_head.png");
	this->entityMesh->assignMaterial(head, 6);

	Material eyes_m;
	eyes_m.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eyemeniscus.png");
	this->entityMesh->assignMaterial(eyes_m, 7);

	Material eye_lash;
	eye_lash.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_eye_lash.png");
	this->entityMesh->assignMaterial(eye_lash, 8);

	Material eye_shadow;
	eye_shadow.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eyeshadow.png");
	this->entityMesh->assignMaterial(eye_shadow, 9);

	Material brow_a;
	brow_a.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_brow.png");
	this->entityMesh->assignMaterial(brow_a, 10);

	Material brow_b;
	brow_b.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_brow2.png");
	this->entityMesh->assignMaterial(brow_b, 11);

	Material beard;
	beard.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_brow.png");
	this->entityMesh->assignMaterial(beard, 12);

	Material teeth;
	teeth.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/gman_teeth.png");
	this->entityMesh->assignMaterial(teeth, 13);

	Material tongue;
	tongue.colorText = TextureManager::getInstance()->loadTextureFromFile("/test/eli_eye_lash.png");
	this->entityMesh->assignMaterial(tongue, 14);

	if (this->entityMesh == nullptr) {
		UreTechEngine::EngineConsole::log("vayRUS cube can not load mesh!", UreTechEngine::EngineConsole::t_error::LOG_ERROR);
	}
	entName = "vayrusCube";
}

vector3 lastPos = { -20.0f,0.0f,0.0f };

void vayrusCube::begin() {
	//add your begin commands here
	print("vayRUS cube Spawned!");
	//this->transform.Location = lastPos;
	//this->transform.Rotation.roll = -90.0f;
	//this->transform.Rotation.yaw = 180.0f;
	//lastPos.x += 1.0f;
}

void vayrusCube::tick() {
	//add your loop commands here
	// this->transform.Rotation.pitch += 0.5f;
}