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
#include "UreTechEngine/utils/string/string.h"
#include <corecrt_io.h>
#include <fcntl.h>

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

#define ImGui_Max_InputChars 512
// editor/game ImGui Vars
bool consoleWindow = false;
bool autoScroll = true;
char consoleInputBuffer[ImGui_Max_InputChars];

std::string engInf;

bool sceneExplorer = false;
entID entToModify = -1;

bool entityTableWindow = false;

bool modifyWindow = false;

bool renameEntWindow = false;
char renameEntInputBuffer[ImGui_Max_InputChars];

//bruh
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

dArray<UreTechEngine::string> parseWith(std::string str, char c) {
	dArray<UreTechEngine::string> res;
	UreTechEngine::string block;
	for (uint64_t i = 0; i < str.size(); i++) {
		if (str[i] != c) {
			block.push_back(str[i]);
		}
		else {
			res.push_back(block);
			block = "";
		}
	}
	res.push_back(block);
	return res;
}

typedef dArray<UreTechEngine::string> conArgs;

typedef void (*conFunc)(conArgs);

struct commandStruct {
	UreTechEngine::string commandName = "";
	conFunc func = nullptr;
	commandStruct(UreTechEngine::string _commandName, conFunc _func) {
		commandName = _commandName;
		func = _func;
	}
};

std::vector<commandStruct> conFuncs;

void executeCommand(UreTechEngine::string commandLine) {
	conArgs args = parseWith(commandLine, ' ');
	UreTechEngine::string command = args[0];
	for (uint64_t i = 1; i < conFuncs.size(); i++) {
		if (command == conFuncs[i].commandName) {
			conFuncs[i].func(args);
			return;
		}
	}
	EngineConsole::log("Invalid command", EngineConsole::ERROR_NORMAL);
}

void con_command_quit(conArgs args) {
	EngineConsole::log("Quiting...", EngineConsole::INFO_NORMAL);
	exit(0);
}

void con_command_info(conArgs args) {
	EngineConsole::log(FULL_ENGINE_DESCRIPTION, EngineConsole::INFO_NORMAL);
}

void con_command_console(conArgs args) {
	if (args.size() == 2) {
		if (args[1] == "free") {
			HWND hwnd = GetConsoleWindow();
			ShowWindow(hwnd, SW_HIDE);
			FILE* f;
			freopen_s(&f, "NUL", "w", stdout);
			freopen_s(&f, "NUL", "r", stdin);
			freopen_s(&f, "NUL", "w", stderr);
			FreeConsole();
			EngineConsole::log("External console closed.", EngineConsole::INFO_NORMAL);
		}
		else if (args[1] == "alloc") {
			HWND hwnd = GetConsoleWindow();
			ShowWindow(hwnd, SW_SHOW);
			AllocConsole();  
			FILE* f;
			freopen_s(&f, "CONOUT$", "w", stdout);
			freopen_s(&f, "CONIN$", "r", stdin);
			freopen_s(&f, "CONOUT$", "w", stderr);
			enableANSI();
			EngineConsole::log("External console opened.", EngineConsole::INFO_NORMAL);
		}
		else {
			EngineConsole::log("Invalid argument!", EngineConsole::ERROR_NORMAL);
		}
	}
	else {
		EngineConsole::log("Unexpected argument!", EngineConsole::ERROR_NORMAL);
	}
}

void initCommands() {
	conFuncs.push_back(commandStruct("quit", con_command_quit));
	conFuncs.push_back(commandStruct("info", con_command_info));
	conFuncs.push_back(commandStruct("console", con_command_console));
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
	if (UPK_ENABLE_PACKAGE_SYSTEM) {
		engine->init_upk_system(UPK_PACKAGE_PATH,UPK_PACKAGE_ENC_KEY);
	}

	initCommands();// init main commands

	// get widow and set callback funcs
	GLFWwindow* window = engine->getWindow();
	engine->setKeyCallBackFunc(key_callback, mouse_button_callback);

	// init net system un used for now
	//UreTechEngine::networkSystem* netSys = UreTechEngine::networkSystem::getNetworkSystem(); 

	player = engine->getPlayer();// store player pointer localy
	TextureManager* textureManager = TextureManager::getInstance();// create texture manager
	MeshManager* meshManager = MeshManager::getInstance();// create mesh manager

	// change window icon
	int icoW, icoH, icoC;
	unsigned char* icoimg = nullptr;
	if (UPK_ENABLE_PACKAGE_SYSTEM) {
		Buffer icoTmpBuf = engine->package->get("/engine/res/icon.png");
		icoimg = stbi_load_from_memory(icoTmpBuf.pointer, icoTmpBuf.size, &icoW, &icoH, &icoC, 0);
	}
	else {
		icoimg = stbi_load("icon.png", &icoW, &icoH, &icoC, 0);
	}
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
	engine->entityConstructors.push_back(entConstructStruct("entity"      , []() { return new entity(); }));
	engine->entityConstructors.push_back(entConstructStruct("vayrusCube"  , []() { return dynamic_cast<entity*>(new vayrusCube()); }));
	engine->entityConstructors.push_back(entConstructStruct("MyPlayerPawn", []() { return dynamic_cast<entity*>(new MyPlayerPawn()); }));
	engInf = "Engine initiated.";
	UreTechEngine::EngineConsole::log("Engine successfuly initiated!", UreTechEngine::EngineConsole::t_error::INFO_NORMAL);

	UreTechEngine::string a = "BRUH MESSAGE";//string test bruh
	UreTechEngine::EngineConsole::log("brub " + a, UreTechEngine::EngineConsole::t_error::DEBUG);

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
		//MENU WINDOW
		ImGui::SetNextWindowSize(ImVec2(display_w, 70));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("vayRUS 3D", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
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
					engInf = "Open console";
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window")) {
				if (ImGui::MenuItem("Scene Explorer")) {

					sceneExplorer = !sceneExplorer;
				}
				if (ImGui::MenuItem("Entity Table")) {

					entityTableWindow = !entityTableWindow;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::Text(engInf.c_str());
		ImGui::End();
		//MENU WINDOW END

		//SCENE EXPLORER WINDOW
		if (sceneExplorer) {
			ImGui::Begin("Scene Explorer", &sceneExplorer);
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImGui::BeginChild("ENTITIES", ImVec2(0, windowSize.y - 100), true);
			for (uint64_t i = 0; i < engine->getCountOfEntity(); i++) {
				ImGui::PushID(i);
				ImGui::Text(std::string(engine->getEntityWithIndex(i)->entClassName + ": " + engine->getEntityWithIndex(i)->entName).c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(windowSize.x - 180);
				if (ImGui::Button("Delete",ImVec2(70,20))) {
					engine->killEntity(engine->getEntityWithIndex(i));
					engInf = "Delete entity";
				}
				ImGui::SetCursorPosX(windowSize.x - 90);
				ImGui::SameLine();
				if (ImGui::Button("Modify", ImVec2(70, 20))) {
					entToModify = engine->getEntityWithIndex(i)->entityID;
					modifyWindow = true;
					engInf = "Modify entity";
				}
				ImGui::PopID();
			}
			ImGui::EndChild();
			ImGui::End();
		}
		//SCENE EXPLORER WINDOW END

		//ENTITY TABLE WINDOW
		if (entityTableWindow) {
			ImGui::Begin("Entity Table", &entityTableWindow);
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImGui::BeginChild("ENTITIES", ImVec2(0, windowSize.y - 100), true);
			for (uint64_t i = 0; i < engine->entityConstructors.size(); i++) {
				ImGui::Text(engine->entityConstructors[i].entClassName.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(windowSize.x - 90);
				ImGui::PushID(i);
				if (ImGui::Button("ADD",ImVec2(70,20))) {
					engine->spawnEntity(engine->entityConstructors[i].constructor());
					engInf = "Add entity";
				}
				ImGui::PopID();
			}
			ImGui::EndChild();
			ImGui::End();
		}
		//ENTITY TABLE WINDOW END

		//MODIFY WINDOW
		if (modifyWindow) {
			ImGui::Begin("Modify", &modifyWindow);
			if (engine->isValidEntity(entToModify)) {
				ImGui::Text(std::string("Entity --> " + engine->getEntityWithID(entToModify)->entName).c_str());
				ImGui::DragScalarN("Location", ImGuiDataType_Double, &engine->getEntityWithID(entToModify)->transform.Location, 3);
				ImGui::DragScalarN("Rotation", ImGuiDataType_Double, &engine->getEntityWithID(entToModify)->transform.Rotation, 3);
				ImGui::DragScalarN("Scale", ImGuiDataType_Double, &engine->getEntityWithID(entToModify)->transform.Scale, 3);
				if (ImGui::Button("Rename Entity")){
					renameEntWindow = true;
					engInf = "Rename entity";
				}
			}
			else {
				ImGui::Text("Invalid entity.");
			}
			ImGui::End();
		}
		//MODIFY WINDOW END

		//RENAME WINDOW
		if (renameEntWindow) {
			ImGui::Begin("Rename Entity", &modifyWindow);
			if (engine->isValidEntity(entToModify)) {
				ImGui::Text(std::string("Entity --> " + engine->getEntityWithID(entToModify)->entName).c_str());
				ImGui::InputText("New Name", renameEntInputBuffer, ImGui_Max_InputChars);
				ImGui::SameLine();
				if (ImGui::Button("Apply")) {
					std::string newName(renameEntInputBuffer, ImGui_Max_InputChars);
					engine->getEntityWithID(entToModify)->entName = newName;
					renameEntInputBuffer[0] = '\0';
					renameEntWindow = false;
					engInf = "Entity renamed";
				}
			}
			else {
				ImGui::Text("Invalid entity.");
			}
			ImGui::End();
		}
		//RENAME WINDOW END
		
	//editor UI end

#endif // ENGINE_BUILD

		// gui build (dedicated server doesn't have ImGui window)
#if defined(ENGINE_BUILD) or defined(GAME_BUILD)
		if (consoleWindow) {
			ImGui::Begin("vConsole 1.0", &consoleWindow);

			ImVec2 windowSize = ImGui::GetWindowSize();


			ImGui::BeginChild("ConsoleOutput", ImVec2(0, windowSize.y - 100), true);
			for (uint64_t i = 0; i < EngineConsole::messages.size(); i++) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(EngineConsole::messages[i].color[0], EngineConsole::messages[i].color[1], EngineConsole::messages[i].color[2], 1.0f));
				if (ImGui::Selectable(EngineConsole::messages[i].msg.c_str(), false)) {
					ImGui::SetClipboardText(EngineConsole::messages[i].msg.c_str());
				}
				ImGui::PopStyleColor();
			}

			if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
			ImGui::EndChild();

			if (ImGui::InputText("Command", consoleInputBuffer, ImGui_Max_InputChars, ImGuiInputTextFlags_EnterReturnsTrue)) {
				std::string conInputStr(consoleInputBuffer); // string saved
				executeCommand(conInputStr);
				consoleInputBuffer[0] = '\0'; // Cleared after bruh
			}

			ImGui::BeginGroup();
			if (ImGui::Button("Clear")) { EngineConsole::messages.clear(); system("cls"); }
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
		if (display_w != 0 && display_h != 0) {
			UreTechEngine::UreTechEngineClass::displayWidth = display_w;
			UreTechEngine::UreTechEngineClass::displayHeight = display_h;
			UreTechEngine::UreTechEngineClass::windowMinmized = false;
		}
		else {
			UreTechEngine::UreTechEngineClass::windowMinmized = true;
		}

		glfwSwapInterval(1);
		glfwSwapBuffers(window);
	}

	return 0;
}