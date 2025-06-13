#include"UreTechEngine/gmodule-dll.h"
#include<UreTechEngine/EngineCore.h>

using namespace UreTechEngine;

ENGINE_GMODULE_DEFINE void module_begin() {
	EngineConsole::log("Module Begin: gmodule-template",EngineConsole::DEBUG);
	//EngineConsole::log("Package name from engine ref -->" + engineReference->package->packageInfo(),EngineConsole::DEBUG);
}

ENGINE_GMODULE_DEFINE void  module_destroy() {

}