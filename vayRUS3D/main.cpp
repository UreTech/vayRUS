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

#define STB_IMAGE_IMPLEMENTATION   
#include<stb/stb_image.h>

#include<../EngineCore.h>

#include "../content/sourceContent/vayrusCube.h"
#include "../content/sourceContent/MyPlayerPawn.h"

#include <thread>
#include<map>

#define fpslock 60
int display_w, display_h;

using namespace std;
using namespace UreTechEngine;

//for debug now
float editorCamRoll=0.0f;
float editorCamYaw = 0.0f;
float editorCamPos[3] = { 0.0f,0.0f,0.0f };

//change this later
bool rClickL = false;

#define ImGui_Max_InputChars 256
// editor/game ImGui Vars
bool consoleWindow = false;
bool autoScroll = true;
char consoleInputBuffer[ImGui_Max_InputChars];

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

void enableANSI() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// start external console
	if (USE_EXTERNAL_CONSOLE) {
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		freopen_s(&f, "CONIN$", "r", stdin);
		enableANSI();
	}

	// init engine
	UreTechEngine::UreTechEngineClass* engine = UreTechEngine::UreTechEngineClass::getEngine();// init engine
	if (engine == nullptr) {
		EngineConsole::log("ENGINE ERROR (0x01)", EngineConsole::ERROR_FATAL);
	}

	// get widow and set callback funcs
	GLFWwindow* window = engine->getWindow();
	engine->setKeyCallBackFunc(key_callback, mouse_button_callback);

	// init net system
	UreTechEngine::networkSystem* netSys = UreTechEngine::networkSystem::getNetworkSystem();

	player = engine->getPlayer();// store player pointer localy
	TextureManager* textureManager = TextureManager::getInstance();// create texture manager
	MeshManager* meshManager = MeshManager::getInstance();// create mesh manager

	// change window icon
	int icoW, icoH, icoC;
	unsigned char* icoimg = stbi_load("icon.png", &icoW, &icoH, &icoC, 0);
	//unsigned char* icoimg = stbi_load_from_memory("content/Textures/icon.png", &icoW, &icoH, &icoC, 0); // used for upk
	GLFWimage icon[1];
	icon[0].width = icoW;
	icon[0].height = icoH;
	icon[0].pixels = icoimg;
	glfwSetWindowIcon(window, 1, icon);

	//******
	/*
	//textures
	Texture0 = textureManager->loadTextureFromFile("content/Textures/susTM.png",false);
	Texture1 = textureManager->loadTextureFromFile("content/Textures/sus.png");
	texture grass01Texture = textureManager->loadTextureFromFile("content/Textures/grass01.jpg");
	texture Texture2 = textureManager->loadTextureFromFile("content/Textures/skysphere01.jpg");
	UreTechEngine::EngineConsole::consoleError(std::to_string(Texture0), UreTechEngine::EngineConsole::INFO_NORMAL);
	UreTechEngine::EngineConsole::consoleError(std::to_string(Texture1), UreTechEngine::EngineConsole::INFO_NORMAL);

	Material susTMMaterial;
	susTMMaterial.colorText = Texture0;

	Material susMaterial;
	susMaterial.colorText = Texture1;

	Material grasMat;
	grasMat.colorText = grass01Texture;

	Material skyspMat;
	skyspMat.colorText = Texture2;

	mesh* mesh0 = meshManager->importMeshFbx("content/Meshs/flaty.obj", grasMat);
	mesh* mesh1 = meshManager->importMeshFbx("content/Meshs/cube2TexTest.obj", susTMMaterial);
	mesh1->useMultipleMaterials = true;
	mesh1->Materials.push_back(susTMMaterial);
	mesh1->Materials.push_back(susMaterial);

	//mesh1->changeLitRender(false);

	//mesh* mesh3 = meshManager->importMeshFbx("content/Meshs/alyx.obj", Texture1);
	mesh* mesh2 = meshManager->importMeshFbx("content/Meshs/skysphere.obj", skyspMat);
	mesh* playerCapsuleMesh = meshManager->importMeshFbx("content/Meshs/defaultCapsule.obj", susTMMaterial);
	*/

	//null for now
	//player->CameraTranform.Location.x = 0.0f;
	//player->CameraTranform.Location.y = -10.0f;
	//player->CameraTranform.Location.z = 3.2f;

	//player->playerPawn = engine->spawnEntity(new MyPlayerPawn(nullptr, "playerPawn",f)); // used for debug

	// prepare ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();

	texture materialThumbTexture = textureManager->loadTextureFromFile("materialThumb.png", false);

	// spawnables
	std::map<std::string, std::function<entity* ()>> spawnables;
	spawnables["entity"] = []() { return new entity(); };
	spawnables["vayrusCube"] = []() { return new vayrusCube(); };

	// main loop
	while (!glfwWindowShouldClose(window)) {
		// render stuff
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		engine->getShaderProgram()->use();

		// mouse input and other stuff
		double xpos, ypos;
		double lxpos, lypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (rClickL) {
			editorCamYaw += lxpos - xpos;
			editorCamRoll += lypos - ypos;

			if (editorCamRoll > 90.0f) {
				editorCamRoll = 90.0f;
			}
			else if (editorCamRoll < -90.0f) {
				editorCamRoll = -90.0f;
			}
		}
		lxpos = xpos;
		lypos = ypos;

		//not useable while player pawn is null
		//player->CameraTranform.Rotation.roll = editorCamRoll;
		//player->CameraTranform.Rotation.yaw = editorCamYaw;
		//player->playerPawn->transform.Rotation.yaw = -1*editorCamYaw;
		//player->playerPawn->transform.Location.x = editorCamPos[0];
		//player->playerPawn->transform.Location.y = editorCamPos[1];
		//player->CameraTranform.Location.z = editorCamPos[2];

		player->updateCamera();

		// tick
		glfwPollEvents();
		engine->engineTick();

		//ui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//editor gui
#ifdef ENGINE_BUILD

		//MAP WINDOW
		ImGui::SetNextWindowSize(ImVec2(display_w, 80));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("vayRUS3D", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Save Game")) {
					//engine->saveGame(std::string(inptmappth));
				}
				if (ImGui::MenuItem("Save Map")) {
					//engine->saveCurrentMap(std::string(inptmappth));
				}
				if (ImGui::MenuItem("Open Game")) {
					//engine->loadGame(std::string(inptmappth));
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Create")) {
				if (ImGui::MenuItem("Material Creator")) {

					//materialWindow = true;
				}

				if (ImGui::MenuItem("Network Test")) {
					//netWindow = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Engine")) {
				if (ImGui::MenuItem("Console")) {

					consoleWindow = !consoleWindow;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		//ImGui::PushItemWidth(200);
		//ImGui::InputText("path", inptmappth, 100);
		//ImGui::SameLine();

		ImGui::End();

		//MAP WINDOW END

	//editor UI end

#endif // ENGINE_BUILD

		// gui build (dedicated server doesn't have ImGui window)
#if defined(ENGINE_BUILD) or defined(GAME_BUILD)
		if (consoleWindow) {
			ImGui::Begin("vConsole 1.0", &consoleWindow);

			ImVec2 windowSize = ImGui::GetWindowSize();


			ImGui::BeginChild("ConsoleOutput", ImVec2(0, windowSize.y - 100), true);
			for (const auto& item : EngineConsole::messages) {
				ImGui::TextColored(ImVec4(item.color[0], item.color[1], item.color[2], 1.0f), item.msg.c_str());
			}

			if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
			ImGui::EndChild();

			ImGui::InputText("Command", consoleInputBuffer, ImGui_Max_InputChars, ImGuiInputTextFlags_EnterReturnsTrue);
			if (ImGui::IsItemActivated()) {
				std::string conInputStr(consoleInputBuffer); // string saved

				consoleInputBuffer[0] = '\0'; // Cleared after bruh
			}

			ImGui::BeginGroup();
			if (ImGui::Button("Clear")) EngineConsole::messages.clear();
			ImGui::SameLine();
			if (ImGui::Button("Copy Last")) EngineConsole::messages[0];
			ImGui::SameLine();
			ImGui::Checkbox("Auto Scroll", &autoScroll);
			ImGui::EndGroup();
			ImGui::End();
		}
#endif // (ENGINE_BUILD) or defined(GAME_BUILD)


	// ImGui render
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// render end stuff
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		UreTechEngine::UreTechEngineClass::displayWidth = display_w;
		UreTechEngine::UreTechEngineClass::displayHeight = display_h;

		glfwSwapInterval(1);
		glfwSwapBuffers(window);
	}

	return 0;
}