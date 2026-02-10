#include "gmodule.h"
#include <UreTechEngine/utils/EngineConsole.h>
#include<UreTechEngine/EngineCore.h>

using namespace UreTechEngine;

typedef void (*module_begin_ptr)();

gmodule gmodule_loader::load_gmodule(const char* modulePath)
{
	EngineConsole::log("Loading gmodule: " + string(modulePath), EngineConsole::DEBUG);
	
	gmodule result;

	result.moduleHandle = LoadLibraryA(modulePath);

	if (!result.moduleHandle) {
		EngineConsole::log("Failed while loading gmodule: " + string(modulePath), EngineConsole::LOG_ERROR);
		return result;
	}

	module_begin_ptr module_begin = (module_begin_ptr)GetProcAddress(result.moduleHandle, "vayRUS3D_GMODULE_BEGIN_FUNC_V0");

	if (module_begin == nullptr) {
		EngineConsole::log("Gmodule does not have begin() entry! " + string(modulePath), EngineConsole::LOG_ERROR);
		return result;
	}

	module_begin();

	EngineConsole::log("Loaded: " + string(modulePath), EngineConsole::DEBUG);

	//...
}

bool gmodule_builder::build_gmodule(const char* module_vcxproj_path) {
	EngineConsole::log("Building gmodule: " + string(module_vcxproj_path), EngineConsole::INFO);
	
	FILE* pipe = _popen((string("D:\\WindowsPrograms\\visualStudio\\MSBuild\\Current\\Bin\\MSBuild.exe ") + module_vcxproj_path + " /p:Configuration=\"Gmodule Debug\" /p:Platform=x64 /v:m 2>&1").c_str(), "r");

	if (!pipe) {
		EngineConsole::log("Failed to build gmodule: " + string(module_vcxproj_path), EngineConsole::LOG_ERROR);
		return false;
	}

	char buf[8192];
	while (fgets(buf, sizeof(buf), pipe)) {
		EngineConsole::log(string(CONSOLE_COLOR_BLUE) + "GBUILDER: " + CONSOLE_COLOR_END + buf, EngineConsole::INFO);
	}

	int result = _pclose(pipe);
	if (!result) {
		EngineConsole::log("Build completed (code 0): " + string(module_vcxproj_path), EngineConsole::INFO);
	}
	else {
		EngineConsole::log("Build failed (code " + u64ToHexStr(result) + "): " + string(module_vcxproj_path), EngineConsole::LOG_ERROR);
	}
}