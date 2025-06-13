//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

//networking
#include<WinSock2.h>

#define STB_IMAGE_IMPLEMENTATION   
#include<stb/stb_image.h>

#include<UreTechEngine/EngineCore.h>

#include<UreTechEngine/utils/string/string.h>

UreTechEngine::UreTechEngineClass* engine = nullptr;

#define fpslock 60
int display_w = 1, display_h = 1;

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
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
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
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
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

//UreTech color format print
void RenderColoredText(std::string text) {
	std::string tmp;
	bool crOpen = false;
	bool isPushed = false;
	bool isSameLineCalled = false;
	for (uint64_t i = 0; i < text.size(); i++) {
			if (crOpen) {
				if (text[i] == '\037') {
					ImGui::Text(tmp.c_str());
					ImGui::SameLine(0.0f, 0.0f);
					isSameLineCalled = true;
					if (isPushed) {
						ImGui::PopStyleColor();
						isPushed = false;
					}
					tmp.clear();
					crOpen = false;
				}
				else {
					tmp.push_back(text[i]);
				}
			}
			else {
				if (text[i] == '\001') {
					ImGui::Text(tmp.c_str());
					ImGui::SameLine(0.0f, 0.0f);
					isSameLineCalled = true;
					tmp.clear();
					crOpen = true;
					i++;

					switch (text[i])
					{
					case 'g':
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));//green
						isPushed = true;
						break;
					case 'r':
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));//red
						isPushed = true;
						break;
					case 'b':
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));//blue
						isPushed = true;
						break;
					case 'y':
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));//yellow
						isPushed = true;
						break;
					case 'o':
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.647f, 0.0f, 1.0f));//orange
						isPushed = true;
						break;
					case 'm':
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 1.0f, 1.0f));//magenta
						isPushed = true;
						break;
					case 'w':
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));//white
						isPushed = true;
						break;
					default:
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));//white
						isPushed = true;
						break;
					}
				}
				else {
					tmp.push_back(text[i]);
				}
			}
		}
	if (!tmp.empty()) {
		ImGui::Text(tmp.c_str());
		//ImGui::SameLine(0.0f, 0.0f);
		isSameLineCalled = false;
		if (isPushed) {
			ImGui::PopStyleColor();
			isPushed = false;
		}
	}
	if (isSameLineCalled) {
		ImGui::NewLine();
	}
}

std::string getCPUInfo() {
	int cpuInfo[4] = { 0 };
	char cpuName[49] = { 0 };

	__cpuid(cpuInfo, 0x80000002);
	memcpy(cpuName, cpuInfo, sizeof(cpuInfo));

	__cpuid(cpuInfo, 0x80000003);
	memcpy(cpuName + 16, cpuInfo, sizeof(cpuInfo));

	__cpuid(cpuInfo, 0x80000004);
	memcpy(cpuName + 32, cpuInfo, sizeof(cpuInfo));

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	int coreCount = sysInfo.dwNumberOfProcessors;

	std::ostringstream info;
	info << "CPU: " << cpuName << " x" << coreCount;
	return info.str();
}

void executeCommand(UreTechEngine::string commandLine) {
	conArgs args = parseWith(commandLine, ' ');
	UreTechEngine::string command = args[0];
	for (uint64_t i = 0; i < EngineConsole::conFuncs.size(); i++) {
		if (command == EngineConsole::conFuncs[i].commandName) {
			EngineConsole::conFuncs[i].func(args);
			return;
		}
	}
	EngineConsole::log("Invalid command '" + command + "'", EngineConsole::LOG_ERROR);
	EngineConsole::log("Type 'help' for commands.", EngineConsole::INFO);
}

void con_command_quit(conArgs args) {
	EngineConsole::log("Quiting...", EngineConsole::INFO);
	exit(0);
}
ENGINE_CONSOLE_COMMAND_REGISTER("quit", con_command_quit, UreTechEngineCommand_quit_index);

void con_command_info(conArgs args) {
	EngineConsole::log(FULL_ENGINE_DESCRIPTION, EngineConsole::INFO);
}
ENGINE_CONSOLE_COMMAND_REGISTER("info", con_command_info, UreTechEngineCommand_info_index);

void con_command_help(conArgs args) {
	EngineConsole::log("All main console commands:", EngineConsole::INFO);
	for (uint64_t i = 0; i < EngineConsole::conFuncs.size(); i++) {
		EngineConsole::log("Command '" + EngineConsole::conFuncs[i].commandName + "'", EngineConsole::INFO);
	}
}
ENGINE_CONSOLE_COMMAND_REGISTER("help", con_command_help, UreTechEngineCommand_help_index);

void con_command_clear(conArgs args) {
	system("cls");
	EngineConsole::messages.clear();
}
ENGINE_CONSOLE_COMMAND_REGISTER("clear", con_command_clear, UreTechEngineCommand_clear_index);

void con_command_gmodule(conArgs args) {
	if (args.size() == 2) {
		gmodule_loader::load_gmodule(args[1].c_str());
	}
	else {
		EngineConsole::log("Invalid argument!", EngineConsole::LOG_ERROR);
	}
}
ENGINE_CONSOLE_COMMAND_REGISTER("gmodule", con_command_gmodule, UreTechEngineCommand_gmodule_index);

void con_command_console(conArgs args) {
	if (args.size() == 2) {
		if (args[1] == "free") {
			HWND hwnd = GetConsoleWindow();
			ShowWindow(hwnd, SW_HIDE);
			EngineConsole::log("External console closed.", EngineConsole::INFO);
			UreTechEngine::UreTechEngineClass::externalConsoleState = false;
		}
		else if (args[1] == "alloc") {
			HWND hwnd = GetConsoleWindow();
			ShowWindow(hwnd, SW_SHOW);
			AllocConsole();  
			SetConsoleTitleA(ENGINE_CONSOLE_TITLE);
			enableANSI();
			EngineConsole::log("External console opened.", EngineConsole::INFO);
			UreTechEngine::UreTechEngineClass::externalConsoleState = true;
		}
		else if (args[1] == "clear") {
			EngineConsole::log("External console cleared.", EngineConsole::INFO);// prints before for not print again
			system("cls");
		}
		else {
			EngineConsole::log("Invalid argument!", EngineConsole::LOG_ERROR);
		}
	}
	else {
		EngineConsole::log("Unexpected argument!", EngineConsole::LOG_ERROR);
	}
}
ENGINE_CONSOLE_COMMAND_REGISTER("console", con_command_console, UreTechEngineCommand_console_index);

void con_command_screen(conArgs args) {
	if (args.size() == 2) {
		if (args[1] == "full") {
			EngineConsole::log("Full screen. (not working for now)", EngineConsole::INFO);
		}
		else {
			EngineConsole::log("Invalid argument!", EngineConsole::LOG_ERROR);
		}
	}
	else {
		EngineConsole::log("Unexpected argument!", EngineConsole::LOG_ERROR);
	}
}
ENGINE_CONSOLE_COMMAND_REGISTER("screen", con_command_screen, UreTechEngineCommand_screen_index);

void con_command_engine(conArgs args) {
	EngineConsole::log("\nScene entity count: " + u64ToDecStr(engine->getCountOfEntity()) + "\nNet mode: " + u64ToDecStr(engine->isInServer), EngineConsole::INFO);
}
ENGINE_CONSOLE_COMMAND_REGISTER("engine", con_command_engine, UreTechEngineCommand_engine_index);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// start external console
	if (USE_EXTERNAL_CONSOLE) {
		AllocConsole();

		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		freopen_s(&f, "CONIN$", "r", stdin);
		freopen_s(&f, "CONOUT$", "w", stderr);

		SetConsoleTitleA(ENGINE_CONSOLE_TITLE);

		UreTechEngineClass::externalConsoleState = true;
		
		executeCommand("console alloc");
	}

	// open external console

	// init engine
	engine = UreTechEngine::UreTechEngineClass::getEngine();// init engine
	glfwMakeContextCurrent(engine->mainRenderer->window);
	ImGui::SetCurrentContext(engine->mainRenderer->ImGuiContext);// set ImGui context for engine

	if (engine == nullptr) {
		EngineConsole::log("ENGINE ERROR (0x01)", EngineConsole::ERROR_FATAL);
	}

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

	stbi_set_flip_vertically_on_load(0);
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

	//player->playerPawn = engine->spawnEntity(ENGINE_GET_REGISTERED_ENTITY("UreTechEngine", "MyPlayerPawn")); // used for debug
#if defined(ENGINE_BUILD) or defined(GAME_BUILD)
	// PRELOAD
	//texture materialThumbTexture = textureManager->loadTextureFromFile("/engine/res/materialThumb.png", false);
	// PRELOAD END
#endif

	// spawnables
	//engine->entityConstructors.push_back(entConstructStruct("entity"      , []() { return new entity(); }));
	//engine->entityConstructors.push_back(entConstructStruct("vayrusCube"  , []() { return dynamic_cast<entity*>(new vayrusCube()); }));
	//engine->entityConstructors.push_back(entConstructStruct("MyPlayerPawn", []() { return dynamic_cast<entity*>(new MyPlayerPawn()); }));
	//engInf = "Engine initiated.";
	UreTechEngine::EngineConsole::log("Engine successfuly initiated!", UreTechEngine::EngineConsole::t_error::INFO);

	UreTechEngine::string a = "BRUH MESSAGE";//string test bruh
	UreTechEngine::EngineConsole::log("brub " + a, UreTechEngine::EngineConsole::t_error::DEBUG);

	/*
	for (uint64_t i0 = 0; i0 < 40; i0++) {
		entity* spwnd = engine->spawnEntity(engine->entityConstructors[1].constructor());
		spwnd->transform.Location.x = -(rand()%40);
		//spwnd->transform.Location.y = rand()%40;
		spwnd->transform.Location.z = -(rand()%40);
	}
	*/
	//engine->spawnEntity(engine->entityConstructors[1].constructor());

	//entity* spwnd = engine->spawnEntity(ENGINE_GET_REGISTERED_ENTITY("UreTechEngine", "vayrusCube"));
	//spwnd->transform.Location.x = 0;
	//spwnd->transform.Location.y = rand()%40;
	//spwnd->transform.Location.z = -2.9;
	//spwnd->transform.Rotation.yaw = -180.0f;
	//spwnd->transform.Rotation.pitch = -180.0f;

	// main loop
#if defined(ENGINE_BUILD) or defined(GAME_BUILD)
	while (!glfwWindowShouldClose(window)) {
#elif DEDICATED_SERVER_BUILD
	while (1) {
#else
#error "UNDEFINED BUILD!"
#endif
	engine->mainRenderer->fsUniforms.lightPos.x = 0;
	engine->mainRenderer->fsUniforms.lightPos.y = 1000.0f;
	engine->mainRenderer->fsUniforms.lightPos.z = 1000.0f;
	engine->mainRenderer->fsUniforms.uLightColor.r = 1.0f;
	engine->mainRenderer->fsUniforms.uLightColor.g = 1.0f;
	engine->mainRenderer->fsUniforms.uLightColor.b = 1.0f;
	engine->mainRenderer->fsUniforms.uLightColor.a = 1.0f;


		// mouse input and other stuff
#if defined(ENGINE_BUILD) or defined(GAME_BUILD)
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
#endif
		//not useable while player pawn is null
		//player->CameraTranform.Rotation.roll = editorCamRoll;
		//player->CameraTranform.Rotation.yaw = editorCamYaw;
		//player->playerPawn->transform.Rotation.yaw = -1*editorCamYaw;
		//player->playerPawn->transform.Location.x = editorCamPos[0];
		//player->playerPawn->transform.Location.y = editorCamPos[1];
		//player->CameraTranform.Location.z = editorCamPos[2];

		//ui
#if defined(ENGINE_BUILD) or defined(GAME_BUILD)
		glfwPollEvents();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
#endif

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
				ImGui::Text((engine->getEntityWithIndex(i)->entClassName + ": " + engine->getEntityWithIndex(i)->entName).c_str());
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
					//vkWaitForFences(engine->mainRenderer->device, 1, &engine->mainRenderer->inFlightFence, VK_TRUE, UINT64_MAX);// wait frame (constructor will be contain vulkan commands)
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
				ImGui::Text(("Entity --> " + engine->getEntityWithID(entToModify)->entName).c_str());
				entity* abasd = engine->getEntityWithID(entToModify);
				ImGui::DragScalarN("Location", ImGuiDataType_Float, &engine->getEntityWithID(entToModify)->transform.Location, 3);
				ImGui::DragScalarN("Rotation", ImGuiDataType_Float, &engine->getEntityWithID(entToModify)->transform.Rotation, 3);
				ImGui::DragScalarN("Scale", ImGuiDataType_Float, &engine->getEntityWithID(entToModify)->transform.Scale, 3);
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
				ImGui::Text(("Entity --> " + engine->getEntityWithID(entToModify)->entName).c_str());
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
			ImGui::Begin(ENGINE_CONSOLE_TITLE, &consoleWindow);

			ImVec2 windowSize = ImGui::GetWindowSize();


			ImGui::BeginChild("ConsoleOutput", ImVec2(0, windowSize.y - 100), true, ImGuiWindowFlags_HorizontalScrollbar);

			if (PRINT_INITIAL_LOGS) {

				RenderColoredText("Initial logs:");

				for (uint64_t i = 0; i < EngineConsole::initialMessages.size(); i++) {
					RenderColoredText(EngineConsole::initialMessages[i].msg.c_str());
				}

				RenderColoredText("Runtime logs:");
			}

			if (EngineConsole::messages.size() < 1000) {
				for (uint64_t i = 0; i < EngineConsole::messages.size(); i++) {
					RenderColoredText(EngineConsole::messages[i].msg.c_str());
				}
			}
			else {
				for (uint64_t i = 1000 - EngineConsole::messages.size(); i < EngineConsole::messages.size(); i++) {
					RenderColoredText(EngineConsole::messages[i].msg.c_str());
				}
			}

			if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
			ImGui::EndChild();

			if (ImGui::InputText("Command", consoleInputBuffer, ImGui_Max_InputChars, ImGuiInputTextFlags_EnterReturnsTrue)) {
				UreTechEngine::string conInputStr(consoleInputBuffer); // string saved
				executeCommand(conInputStr);
				consoleInputBuffer[0] = '\0'; // Cleared after bruh
			}

			ImGui::BeginGroup();
			if (ImGui::Button("Clear")) {
				executeCommand("clear");
			}
			ImGui::SameLine();
			ImGui::Checkbox("Auto Scroll", &autoScroll);
			ImGui::EndGroup();
			ImGui::End();
		}

		ImGui::Render();

		// render stuff

		glfwGetFramebufferSize(window, &display_w, &display_h);
		if (display_w != 0 && display_h != 0) {
			UreTechEngine::UreTechEngineClass::displayWidth = display_w;
			UreTechEngine::UreTechEngineClass::displayHeight = display_h;
			UreTechEngine::UreTechEngineClass::windowMinmized = false;
		}
		else {
			UreTechEngine::UreTechEngineClass::windowMinmized = true;
			display_w = 1;
			display_h = 1;
		}

		// wait frame
		engine->mainRenderer->waitFrame();

		if (!engine->windowMinmized) {

			engine->mainRenderer->recrateSwapChainIfNeeded();
			engine->mainRenderer->prepareNewFrame();

			//ENGINE VULKAN RENDER
			engine->mainRenderer->BeginVulkanCommandBuffer();

			player->updateCamera();

			engine->engineTick();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), engine->mainRenderer->EngineCommandBuffer);

			engine->mainRenderer->EndVulkanCommandBuffer();
			//ENGINE VULKAN RENDER END

			// processing...

			engine->mainRenderer->processCommandBuffers();

			engine->mainRenderer->endFrame();

			// present 
			vkQueuePresentKHR(engine->mainRenderer->presentQueue, &engine->mainRenderer->presentInfo);
		}
#endif // (ENGINE_BUILD) or defined(GAME_BUILD)

		//glfwSwapInterval(0);
		//glfwSwapBuffers(window);
	}
	exit(0);
	//vkDeviceWaitIdle(engine->mainRenderer->device);
	//engine->mainRenderer->destroyVulkan();
	return 0;
}