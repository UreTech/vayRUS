#include <UreTechEngine/utils/EngineRegisters.h>

namespace UreTechEngine {
	dArray<entConstructStruct> UreTechEngineClass::entityConstructors;
}

size_t uEngine_reg_addEntity(UreTechEngine::string entClassName, entConstructFunc constructor, UreTechEngine::string entClassNamespace)
{
	bool found = false;
	for (size_t i = 0; i < UreTechEngineClass::entityConstructors.size(); i++) {
		if (UreTechEngineClass::entityConstructors.at(i).entClassNamespace == entClassNamespace) {
			if (UreTechEngineClass::entityConstructors.at(i).entClassName == entClassName) {
				found = true;
				return i;
				break;
			}
		}
		
	}

	if (!found) {
		EngineConsole::initial_log(CONSOLE_COLOR_GREEN + entClassNamespace + CONSOLE_COLOR_END + CONSOLE_COLOR_WHITE + "::" + CONSOLE_COLOR_END + CONSOLE_COLOR_BLUE + entClassName + CONSOLE_COLOR_END + CONSOLE_COLOR_WHITE " added to engine entity registry." + CONSOLE_COLOR_END, EngineConsole::DEBUG);
		return UreTechEngineClass::entityConstructors.push_back(entConstructStruct(
			entClassName,
			constructor,
			entClassNamespace
		));
	}
}

UreTechEngine::entity* uEngine_reg_getEntityWithName(UreTechEngine::string entClassNamespace, UreTechEngine::string entClassName) {
	// returns first entity if not found
	entConstructStruct foundOne = UreTechEngineClass::entityConstructors.at(0);
	bool found = false;

	//find
	for (size_t i = 0; i < UreTechEngineClass::entityConstructors.size(); i++) {
		if (UreTechEngineClass::entityConstructors.at(i).entClassNamespace == entClassNamespace) {
			if (UreTechEngineClass::entityConstructors.at(i).entClassName == entClassName) {
				foundOne = UreTechEngineClass::entityConstructors.at(i);
				found = true;
				break;
			}
		}

	}

	if (!found) {
		EngineConsole::log(CONSOLE_COLOR_GREEN + entClassNamespace + CONSOLE_COLOR_END + CONSOLE_COLOR_WHITE + "::" + CONSOLE_COLOR_END + CONSOLE_COLOR_BLUE + entClassName + CONSOLE_COLOR_END + " not found in engine registers. Returning first entity constructor as default.", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}

	return foundOne.constructor();
}

ENGINE_DEFINE size_t uEngine_reg_addConsoleCommand(UreTechEngine::string _commandName, UreTechEngine::conFunc _func)
{
	for (size_t i = 0; i < EngineConsole::conFuncs.size(); i++) {
		if (EngineConsole::conFuncs.at(i).commandName == _commandName) {
			return i;
		}
	}
	return EngineConsole::conFuncs.push_back(commandStruct(_commandName, _func));
}
