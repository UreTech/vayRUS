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
		};
		static void consoleError(std::string _errMsg,t_error errTyp);
	private:
		static void forceToExit();
		EngineConsole();
	};
}

#endif // !errOut_h
