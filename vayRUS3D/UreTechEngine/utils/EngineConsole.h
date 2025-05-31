#pragma once
#include<stdio.h>
#include <../EngineCore.h>
using namespace UreTechEngine;

namespace UreTechEngine {
	struct conMessage {
		UreTechEngine::string msg;// message
		float color[3];// rgb color
	};

	class EngineConsole {
	public:
		enum t_error {
			ERROR_ERROR = -1,
			ERROR_FATAL,
			ERROR_NORMAL,
			WARN_NORMAL,
			WARN_CAN_CAUSE_ERROR,
			INFO_NORMAL,
			DEBUG,
		};
		static void log(UreTechEngine::string logMessage,t_error logType);
		static UreTechEngine::dArray<conMessage> messages;
	private:
		static void forceToExit();
		EngineConsole();
	};
}