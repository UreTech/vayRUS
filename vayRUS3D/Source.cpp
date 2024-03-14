#define Editor_Mode true

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

//networking
#include<WinSock2.h>
#include"UreTechEngine/network/network.h"

#define STB_IMAGE_IMPLEMENTATION   
#include<stb/stb_image.h>
 
#include"UreTechEngine/shaders/vertexStructs.hpp"
#include"UreTechEngine/EngineBase.h"
#include"UreTechEngine/shaders/shaderSystem.hpp"
#include"UreTechEngine/shaders/textureSystem.hpp"
#include"UreTechEngine/shaders/vertexArray.hpp"
#include"UreTechEngine/shaders/MeshManager.hpp"
#include"UreTechEngine/shaders/mesh.hpp"
#include"UreTechEngine/utils/3DMath.hpp"
#include"UreTechEngine/utils/Array.hpp"

#include"UreTechEngine/network/network.h"


#include "../content/sourceContent/vayrusCube.h"
#include "../content/sourceContent/MyPlayerPawn.h"

#include <thread>
#include<map>
#include "UreTechEngine/utils/errOut.h"


vertexArrayObject EngineTestCubeVao;
vertexArrayObject EngineTestPyramidVao;

#define fpslock 60
int display_w, display_h;
bool isEditor = 0;
using namespace std;
using namespace UreTechEngine;

entity* TestDumy = nullptr;

float editorCamRoll=0.0f;
float editorCamYaw = 0.0f;

float editorCamPos[3] = { 0.0f,0.0f,0.0f };

bool rClickL = false;

UreTechEngine::Player* player = nullptr;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	switch (key)
	{
	case(GLFW_KEY_UP):
		editorCamYaw += 1;
		break;
	case(GLFW_KEY_DOWN):
		editorCamYaw += -1;
		break;
	case(GLFW_KEY_LEFT):
		editorCamRoll += 1;
		break;
	case(GLFW_KEY_RIGHT):
		editorCamRoll += -1;
		break;
	}

	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window,1);
	}
	if (key == GLFW_KEY_W && rClickL) {
		vector3 change = Math3D::addWithRotation(player->CameraTranform.Rotation, vector3(1.0f, 0.0f, 0.0f), player->playerPawn->transform.Location);
		editorCamPos[0] = change.fx();
		editorCamPos[1] = change.fy();
	}
	if (key == GLFW_KEY_S && rClickL) {
		vector3 change = Math3D::addWithRotation(player->CameraTranform.Rotation, vector3(-1.0f, 0.0f, 0.0f), player->playerPawn->transform.Location);
		editorCamPos[0] = change.fx();
		editorCamPos[1] = change.fy();
	}

	if (key == GLFW_KEY_D && rClickL) {
		vector3 change = Math3D::addWithRotation(player->CameraTranform.Rotation, vector3(0.0f, 1.0f, 0.0f), player->playerPawn->transform.Location);
		editorCamPos[0] = change.fx();
		editorCamPos[1] = change.fy();
	}
	if (key == GLFW_KEY_A && rClickL) {
		vector3 change = Math3D::addWithRotation(player->CameraTranform.Rotation, vector3(0.0f, -1.0f, 0.0f), player->playerPawn->transform.Location);
		editorCamPos[0] = change.fx();
		editorCamPos[1] = change.fy();
	}
	
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (rClickL) {
			rClickL = false;
		}
		else {
			rClickL = true;
		}
	}
}

texture Texture0;
texture Texture1;

int main(int argc, char** argv) {
	//init engine
	UreTechEngine::UreTechEngineClass* engine = UreTechEngine::UreTechEngineClass::getEngine();//init engine
	GLFWwindow* window = engine->getWindow();
	engine->setKeyCallBackFunc(key_callback, mouse_button_callback);
	//init net system
	UreTechEngine::networkSystem* netSys = UreTechEngine::networkSystem::getNetworkSystem();

	player = engine->getPlayer();//get player ref
	TextureManager* textureManager = TextureManager::getInstance();//create texture manager
	MeshManager* meshManager = MeshManager::getInstance();
	int icoW, icoH, icoC;
	unsigned char* icoimg = stbi_load("content/Textures/icon.png", &icoW, &icoH, &icoC, 0);
	GLFWimage icon[1];
	icon[0].width = icoW;
	icon[0].height = icoH;
	icon[0].pixels = icoimg;
	glfwSetWindowIcon(window, 1, icon);
	//******

	//textures
	Texture0 = textureManager->loadTextureFromFile("content/Textures/susTM.png",false);
	Texture1 = textureManager->loadTextureFromFile("content/Textures/sus.png");
	texture grass01Texture = textureManager->loadTextureFromFile("content/Textures/grass01.jpg");
	texture Texture2 = textureManager->loadTextureFromFile("content/Textures/skysphere01.jpg");
	UreTechEngine::EngineERROR::consoleError(std::to_string(Texture0), UreTechEngine::EngineERROR::INFO_NORMAL);
	UreTechEngine::EngineERROR::consoleError(std::to_string(Texture1), UreTechEngine::EngineERROR::INFO_NORMAL);



	mesh* mesh0 = meshManager->importMeshFbx("content/Meshs/flaty.obj", grass01Texture);
	mesh* mesh1 = meshManager->importMeshFbx("content/Meshs/cube2TexTest.obj", Texture0);
	mesh1->useMultipleTexture = true;
	mesh1->textures.push_back(Texture0); 
	mesh1->textures.push_back(Texture1);

	//mesh1->changeLitRender(false);

	//mesh* mesh3 = meshManager->importMeshFbx("content/Meshs/alyx.obj", Texture1);
	mesh* mesh2 = meshManager->importMeshFbx("content/Meshs/skysphere.obj", Texture2);

	mesh* playerCapsuleMesh = meshManager->importMeshFbx("content/Meshs/defaultCapsule.obj", Texture0);

	player->CameraTranform.Location.x = 0.0f;
	player->CameraTranform.Location.y = -10.0f;
	player->CameraTranform.Location.z = 3.2f;

	Transform3D a(vector3(0.0f, 0.0f, -1.8f), Rotation(0.0f, 0.0f, 0.0f), vector3(60.0f, 60.0f, 60.0f));
	Transform3D b(vector3(-1.0f, 1.0f, -0.6f), Rotation(90.0f, 0.0f, 0.0f), vector3(0.0f, 0.0f, 0.0f));
	Transform3D c(vector3(1.0f, -1.0f, -0.6f), Rotation(0.0f, 0.0f, 0.0f), vector3(0.0f, 0.0f, 0.0f));
	Transform3D d(vector3(-1.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f));
	Transform3D e(vector3(0.0f, 0.0f, 0.0f), Rotation(-30.0f, -90.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f));
	Transform3D f(vector3(0.0f, 0.0f, 1.0f), Rotation(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f));

	TestDumy = engine->spawnEntity(new entity(a.Location,a.Rotation,a.Scale, mesh0,"flat"));
	//engine->spawnEntity(new entity(b.Location, b.Rotation, b.Scale, mesh1, "cube0"));
	//engine->spawnEntity(new entity(c.Location, c.Rotation, c.Scale, mesh3, "cube1"));
	engine->spawnEntity(new entity(d.Location, d.Rotation, d.Scale, playerCapsuleMesh, "cube2"));
	mesh2->changeLitRender(false);
	engine->spawnEntity(new entity(e.Location, e.Rotation, e.Scale, mesh2, "skysphere"));

	player->playerPawn = engine->spawnEntity(new MyPlayerPawn(nullptr, "playerPawn",f));
	

	float rot = 0;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();

	Transform3D editorEditTransform(vector3(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f));
	float editorTransformLoc[3] = { 0.0f,0.0f,0.0f };
	float editorTransformRot[3] = { 0.0f,0.0f,0.0f };
	float editorTransformScl[3] = { 1.0f,1.0f,1.0f };

	entity* selectedEntityEngine = nullptr;

	//net ui vars
	char toConnectIP_UIChar[150];
	char toConnectIPPORT_UIChar[150];
	memset(toConnectIP_UIChar, 0, 150);
	memset(toConnectIPPORT_UIChar, 0, 150);


	//editor spawnables
	std::map<std::string, std::function<entity* ()>> spawnables;

	spawnables["entity"] = []() { return new entity(); };
	spawnables["vayrusCube"] = []() { return new vayrusCube(); };

	networkReplicationStruct* netTest = new networkReplicationStruct();
	networkReplicationStruct* netTest2 = new networkReplicationStruct();
	memcpy(&netTest->func_notify[0],"func_rep_Test",13);
	netSys->replicating_func_dat_o = netTest;
	netSys->replicating_func_dat_i = netTest2;
	netSys->replicating_func_dat_size = sizeof(networkReplicationStruct);

	//while (1) {}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		engine->getShaderProgram()->use();

		double xpos, ypos;
		double lxpos, lypos;

		glfwGetCursorPos(window, &xpos, &ypos);
		if (rClickL) {
			editorCamYaw += lxpos - xpos;
			editorCamRoll += lypos - ypos;

			if (editorCamRoll > 90.0f) {
				editorCamRoll = 90.0f;
			}else if (editorCamRoll < -90.0f) {
				editorCamRoll = -90.0f;
			}
		}
		//end of exec
		lxpos = xpos;
		lypos = ypos;

		player->CameraTranform.Rotation.roll = editorCamRoll;
		player->CameraTranform.Rotation.yaw = editorCamYaw;

		player->playerPawn->transform.Rotation.yaw = -1*editorCamYaw;

		player->playerPawn->transform.Location.x = editorCamPos[0];
		player->playerPawn->transform.Location.y = editorCamPos[1];
		//player->CameraTranform.Location.z = editorCamPos[2];

		player->updateCamera();

		glfwPollEvents();
		engine->engineTick();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//editor UI
		if(Editor_Mode)
			{
			//SPAWN WINDOW
			ImGui::SetNextWindowPos(ImVec2(display_w - 450, display_h - 880));
			ImGui::SetNextWindowSize(ImVec2(450, 80));
			ImGui::Begin("Creator", NULL, ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
			char editorSpawnTextInput[150];
			memset(editorSpawnTextInput, 0, 150);
			ImGui::Text("Class Name:");
			if (ImGui::InputText("class", editorSpawnTextInput, 150, ImGuiInputTextFlags_EnterReturnsTrue)) {
					auto it = spawnables.find(string(editorSpawnTextInput));
					if (it != spawnables.end()) {
						entity* spawned = engine->spawnEntity(it->second());
					}
			}

			ImGui::End();
			//SPAWN WINDOW END

			//EXPLORER WINDOW
			ImGui::SetNextWindowPos(ImVec2(display_w - 450, display_h - 800));
			ImGui::SetNextWindowSize(ImVec2(450, 500));
			ImGui::Begin("Explorer", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

			for (int i = 0; i < engine->getCountOfEntity(); i++) {
				ImGui::PushItemWidth(450);
				ImGui::Text(engine->getEntityWithIndex(i)->entName.c_str());
				ImGui::SameLine();
				ImGui::PushID(to_string(i).c_str());
				if (ImGui::Button(string(string("select ") + engine->getEntityWithIndex(i)->entName).c_str())) {
					selectedEntityEngine = engine->getEntityWithIndex(i);
					editorEditTransform = engine->getEntityWithIndex(i)->transform;
					editorTransformLoc[0] = editorEditTransform.Location.fx();
					editorTransformLoc[1] = editorEditTransform.Location.fy();
					editorTransformLoc[2] = editorEditTransform.Location.fz();

					editorTransformRot[0] = editorEditTransform.Rotation.fRoll();
					editorTransformRot[1] = editorEditTransform.Rotation.fPitch();
					editorTransformRot[2] = editorEditTransform.Rotation.fYaw();

					editorTransformScl[0] = editorEditTransform.Scale.fx();
					editorTransformScl[1] = editorEditTransform.Scale.fy();
					editorTransformScl[2] = editorEditTransform.Scale.fz();
				}
				ImGui::SameLine();
				if (ImGui::Button(string(string("destroy ") + engine->getEntityWithIndex(i)->entName).c_str())) {
					engine->killEntity(engine->getEntityWithIndex(i));
					selectedEntityEngine = nullptr;
				}
				ImGui::PopID();
			}
			ImGui::End();
			//EXPLORER WINDOW END

			//TRANSFORM WINDOW
			ImGui::SetNextWindowPos(ImVec2(display_w - 450, display_h - 300));
			ImGui::SetNextWindowSize(ImVec2(450, 300));
			if (selectedEntityEngine == nullptr) {
				ImGui::Begin("Transform", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			}

			if (selectedEntityEngine != nullptr) {
				ImGui::Begin(string(string("Transform ") + selectedEntityEngine->entName).c_str(), &isEditor, ImGuiWindowFlags_NoMove);
				ImGui::Text("Location");
				ImGui::PushItemWidth(360);
				ImGui::DragFloat3("Location", editorTransformLoc,0.1f);
				ImGui::PushItemWidth(360);
				ImGui::Text("Rotation");
				ImGui::DragFloat3("Rotation", editorTransformRot,2.0f);
				ImGui::PushItemWidth(360);
				ImGui::Text("Scale");
				ImGui::DragFloat3("Scale", editorTransformScl, 0.1f);
			}
			ImGui::End();
			//TRANSFORM WINDOW END
		}
		//editor UI end

		//NET WINDOW
		bool netWindow = 1;
		ImGui::Begin("Connect To Server", &netWindow,ImGuiWindowFlags_NoCollapse);

		ImGui::Text("CONNECT:");
		ImGui::InputText("IP", toConnectIP_UIChar, 150, ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::InputText("PORT", toConnectIPPORT_UIChar, 150, ImGuiInputTextFlags_EnterReturnsTrue);
		if (ImGui::Button("CONNECT")) {
				memcpy(toConnectIP_UIChar, "127.0.0.1", 9);
				memcpy(toConnectIPPORT_UIChar, "80", 2);
				netSys->setToConnectIPAddr(std::string(toConnectIP_UIChar),std::string(toConnectIPPORT_UIChar));
				memset(toConnectIP_UIChar, 0, 150);
				memset(toConnectIPPORT_UIChar, 0, 150);
				netSys->connectToServer();	
		}
		ImGui::End();

		ImGui::Begin("Host a Server", &netWindow, ImGuiWindowFlags_NoCollapse);

		ImGui::Text("Host:");
		//ImGui::InputText("PORT", toConnectIPPORT_UIChar, 150, ImGuiInputTextFlags_EnterReturnsTrue);
		if (ImGui::Button("Host")) {
			netSys->startServer();
		}
		ImGui::End();
		//NET WINDOW END
		if (engine->isServer && engine->isInServer) {
			netTest->func_test_input.test = "im tstsr!";
			netSys->connectionRequest();
			netSys->sendRecvToClient();
		}
		if (!engine->isServer && engine->isInServer) {
			netSys->sendRecvToServer();
			netTest2->run_noticed_funcs();
		}


		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		if (selectedEntityEngine != nullptr) {
			editorEditTransform.Location.x = editorTransformLoc[0];
			editorEditTransform.Location.y = editorTransformLoc[1];
			editorEditTransform.Location.z = editorTransformLoc[2];

			editorEditTransform.Rotation.roll = editorTransformRot[0];
			editorEditTransform.Rotation.pitch = editorTransformRot[1];
			editorEditTransform.Rotation.yaw = editorTransformRot[2];

			editorEditTransform.Scale.x = editorTransformScl[0];
			editorEditTransform.Scale.y = editorTransformScl[1];
			editorEditTransform.Scale.z = editorTransformScl[2];

			selectedEntityEngine->transform = editorEditTransform;
		}

		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		UreTechEngine::UreTechEngineClass::displayWidth = display_w;
		UreTechEngine::UreTechEngineClass::displayHeight = display_h;

		glfwSwapInterval(1);
		glfwSwapBuffers(window);
	}

	return 0;
}