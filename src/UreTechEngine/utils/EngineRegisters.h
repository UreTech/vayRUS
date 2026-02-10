#include<UreTechEngine/EngineCore.h>
#include<UreTechEngine/EngineBase.h>
//#include<UreTechEngine/utils/dArray.h>

namespace UreTechEngine {
	class entity;
	typedef entity* (*entConstructFunc)();
	struct entConstructStruct;

	typedef dArray<UreTechEngine::string> conArgs;
	typedef void (*conFunc)(conArgs);
}

// NOTE: dont use this functions, use the macros instead!
ENGINE_DEFINE size_t uEngine_reg_addEntity(UreTechEngine::string entClassName, UreTechEngine::entConstructFunc constructor, UreTechEngine::string entClassNamespace);
ENGINE_DEFINE UreTechEngine::entity* uEngine_reg_getEntityWithName(UreTechEngine::string entClassNamespace, UreTechEngine::string entClassName);
ENGINE_DEFINE size_t uEngine_reg_addConsoleCommand(UreTechEngine::string _commandName, UreTechEngine::conFunc _func);
