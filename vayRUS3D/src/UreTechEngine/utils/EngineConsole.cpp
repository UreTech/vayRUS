#include "EngineConsole.h"
#include<UreTechEngine/utils/string/string.h>
#include<UreTechEngine/EngineCore.h>

#include <string>
#include <iostream>
#include<conio.h>
#include <Windows.h>

using namespace std;
using namespace UreTechEngine;
namespace UreTechEngine {
	class EngineConsole;
}

#define RESET   "\033[0m" 
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"

UreTechEngine::dArray<conMessage> UreTechEngine::EngineConsole::messages = {};
UreTechEngine::dArray<conMessage> UreTechEngine::EngineConsole::initialMessages = {};
UreTechEngine::dArray<commandStruct> UreTechEngine::EngineConsole::conFuncs = {};

bool isPrintBusy = false;

void UreTechEngine::EngineConsole::log(UreTechEngine::string logMessage, t_error logType)
{
	while (isPrintBusy);
	isPrintBusy = true; // prevent multiple threads from printing at the same time
	conMessage msg;
	std::wstring a;
	switch (logType)
	{
	case UreTechEngine::EngineConsole::ERROR_ERROR:
		cout << "(" << BLUE << "?" << RESET << ")[" << BLUE << "UNKNOWN ERROR" << RESET << "] " << logMessage.std_str() << endl;
		msg.msg = "(?)[UNKNOWN ERROR] " + logMessage;
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::ERROR_FATAL:
		cout << "("<< RED << "!" << RESET <<")[" << RED << "FATAL ERROR" << RESET << "] " << logMessage.std_str() << endl;
		msg.msg = "(\x01r!\x1F)[\x01rFATAL ERROR\x1F] " + logMessage;// UreTech color format
		EngineConsole::messages.push_back(msg);
		a = L"(!)FATAL ERROR:" + std::wstring(logMessage.data(), logMessage.data() + logMessage.lenght());
		MessageBox(NULL, a.c_str(), L"Internal Engine ERROR!", MB_ICONERROR);
		exit(-1);
		break;
	case UreTechEngine::EngineConsole::LOG_ERROR:
		cout << "(" << RED << "i" << RESET << ")[" << RED << "ERROR" << RESET << "] " << logMessage.std_str() << endl;
		msg.msg = "(\x01ri\x1F)[\x01rERROR\x1F] " + logMessage;// UreTech color format
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::WARN:
		cout << "(" << BRIGHT_YELLOW << "i" << RESET << ")[" << BRIGHT_YELLOW << "WARN" << RESET << "] " << logMessage.std_str() << endl;
		msg.msg = "(\x01yi\x1F)[\x01yWARN\x1F] " + logMessage;// UreTech color format
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::WARN_CAN_CAUSE_ERROR:
		cout << "(" << YELLOW << "!" << RESET << ")[" << YELLOW << "WARN" << RESET << "] " << logMessage.std_str() << endl;
		msg.msg = "(\x01y!\x1F)[\x01yWARN\x1F] " + logMessage;// UreTech color format
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::INFO:
		cout << "(" << GREEN << "i" << RESET << ")[" << GREEN << "INFO" << RESET << "] " << logMessage.std_str() << endl;
		msg.msg = "(\x01gi\x1F)[\x01gINFO\x1F] " + logMessage;// UreTech color format
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::DEBUG:
		cout << "(" << MAGENTA << "D" << RESET << ")[" << MAGENTA << "DEBUG" << RESET << "] " << logMessage.std_str() << endl;
		msg.msg = "(\x01mD\x1F)[\x01mDEBUG\x1F] " + logMessage;// UreTech color format
		EngineConsole::messages.push_back(msg);
		break;
	default:
		cout << BRIGHT_BLUE << "UNKNOWN: an unknown error reported! check your code." << RESET << endl;
		msg.msg = "\x01oUNKNOWN: an unknown error reported! check your code.\x1F";
		EngineConsole::messages.push_back(msg);
		break;
	}
	isPrintBusy = false; // allow other threads to print
}

void UreTechEngine::EngineConsole::initial_log(UreTechEngine::string logMessage, t_error logType)
{
	conMessage msg;
	std::wstring a;
	switch (logType)
	{
	case UreTechEngine::EngineConsole::ERROR_ERROR:
		msg.msg = "(?)[UNKNOWN ERROR] " + logMessage;
		EngineConsole::initialMessages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::ERROR_FATAL:
		msg.msg = "(\x01r!\x1F)[\x01rFATAL ERROR\x1F] " + logMessage;// UreTech color format
		EngineConsole::initialMessages.push_back(msg);
		a = L"(!)FATAL ERROR:" + std::wstring(logMessage.data(), logMessage.data() + logMessage.lenght());
		MessageBox(NULL, a.c_str(), L"Internal Engine ERROR!", MB_ICONERROR);
		exit(-1);
		break;
	case UreTechEngine::EngineConsole::LOG_ERROR:
		msg.msg = "(\x01ri\x1F)[\x01rERROR\x1F] " + logMessage;// UreTech color format
		EngineConsole::initialMessages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::WARN:
		msg.msg = "(\x01yi\x1F)[\x01yWARN\x1F] " + logMessage;// UreTech color format
		EngineConsole::initialMessages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::WARN_CAN_CAUSE_ERROR:
		msg.msg = "(\x01y!\x1F)[\x01yWARN\x1F] " + logMessage;// UreTech color format
		EngineConsole::initialMessages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::INFO:
		msg.msg = "(\x01gi\x1F)[\x01gINFO\x1F] " + logMessage;// UreTech color format
		EngineConsole::initialMessages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::DEBUG:
		msg.msg = "(\x01mD\x1F)[\x01mDEBUG\x1F] " + logMessage;// UreTech color format
		EngineConsole::initialMessages.push_back(msg);
		break;
	default:
		msg.msg = "\x01oUNKNOWN: an unknown error reported! check your code.\x1F";
		EngineConsole::initialMessages.push_back(msg);
		break;
	}
}

void UreTechEngine::EngineConsole::forceToExit()
{
	cout << "press esc to exit..." << endl;
	while (_getch()!= 27) {}
	exit(-1);
}

void strLogFunc(const char* _msgStr, bool isFatal) {
	if (isFatal) {
		UreTechEngine::EngineConsole::log(_msgStr, UreTechEngine::EngineConsole::t_error::ERROR_FATAL);
	}
	else {
		UreTechEngine::EngineConsole::log(_msgStr, UreTechEngine::EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
	}
}

uStrConsoleLogFuncPtr UreTechEngine::strLog = strLogFunc;