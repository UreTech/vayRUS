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

	module_begin_ptr module_begin = (module_begin_ptr)GetProcAddress(result.moduleHandle, "module_begin");
	module_begin();

	EngineConsole::log("Loading begun: " + string(modulePath), EngineConsole::DEBUG);

	//...
}
