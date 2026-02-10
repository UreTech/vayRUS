#pragma once
#include<stdio.h>
#include<UreTechEngine/EngineCore.h>
using namespace UreTechEngine;

#define CONSOLE_COLOR_GREEN   "\001g"
#define CONSOLE_COLOR_RED     "\001r"
#define CONSOLE_COLOR_BLUE    "\001b"
#define CONSOLE_COLOR_YELLOW  "\001y"
#define CONSOLE_COLOR_ORANGE  "\001o"
#define CONSOLE_COLOR_MAGENTA "\001m"
#define CONSOLE_COLOR_WHITE   "\001w"
#define CONSOLE_COLOR_END     "\037"

#define ENGINE_CONSOLE_COMMAND_REGISTER(COMMAND_NAME, COMMAND_FUNCTION, INDEX_VAR_NAME) \
	static uint64_t INDEX_VAR_NAME = []() { return uEngine_reg_addConsoleCommand(COMMAND_NAME, COMMAND_FUNCTION);}();


namespace UreTechEngine {
	struct conMessage {
		UreTechEngine::string msg;// message
	};

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

	class ENGINE_DEFINE EngineConsole {
	public:
		enum t_error {
			ERROR_ERROR = -1,
			ERROR_FATAL,
			LOG_ERROR,
			WARN,
			WARN_CAN_CAUSE_ERROR,
			INFO,
			DEBUG,
		};

		// in runtime console
		static void log(UreTechEngine::string logMessage, t_error logType);

		// used in intial contructor calls, so it can be used before the engine is fully initialized
		static void initial_log(UreTechEngine::string logMessage, t_error logType);

		static UreTechEngine::dArray<conMessage> messages;
		static UreTechEngine::dArray<conMessage> initialMessages;

		static dArray<commandStruct> conFuncs;
	private:
		static void forceToExit();
		EngineConsole();
	};
}
