#ifndef errOut_h
#define errOut_h
#include<stdio.h>
#include <string>

namespace UreTechEngine {
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
		static void log(std::string logMessage,t_error logType);
	private:
		static void forceToExit();
		EngineConsole();
	};
}

#endif // !errOut_h
