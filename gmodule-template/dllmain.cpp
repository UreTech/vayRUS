#include"UreTechEngine/gmodule-dll.h"
#include<UreTechEngine/EngineCore.h>

using namespace UreTechEngine;

MODULE_EXPORT void module_begin() {
	EngineConsole::log("Module Begin: gmodule-template",EngineConsole::DEBUG);
	//EngineConsole::log("Package name from engine ref -->" + engineReference->package->packageInfo(),EngineConsole::DEBUG);
}

MODULE_EXPORT void  module_destroy() {

}
