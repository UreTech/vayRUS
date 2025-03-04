#include<../EngineCore.h>
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

std::vector<conMessage> UreTechEngine::EngineConsole::messages = {};

void UreTechEngine::EngineConsole::log(std::string logMessage, t_error logType)
{
	std::wstring a = L"(!)FATAL ERROR:" + std::wstring(logMessage.begin(), logMessage.end());
	conMessage msg;
	switch (logType)
	{
	case UreTechEngine::EngineConsole::ERROR_ERROR:
		cout << "(" << BLUE << "?" << RESET << ")[" << BLUE << "UNKNOWN ERROR" << RESET << "] " << logMessage << endl;
		msg.msg = "(?)[UNKNOWN ERROR] " + logMessage;
		msg.color[0] = 0;
		msg.color[1] = 0;
		msg.color[2] = 1.0f;
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::ERROR_FATAL:
		cout << "("<< RED << "!" << RESET <<")[" << RED << "FATAL ERROR" << RESET << "] " << logMessage << endl;
		msg.msg = "(!)[FATAL ERROR] " + logMessage;
		msg.color[0] = 1.0f;
		msg.color[1] = 0;
		msg.color[2] = 0;
		EngineConsole::messages.push_back(msg);
		MessageBox(NULL, a.c_str(), L"Internal Engine ERROR!", MB_ICONERROR);
		forceToExit();
		break;
	case UreTechEngine::EngineConsole::ERROR_NORMAL:
		cout << "(" << RED << "i" << RESET << ")[" << RED << "ERROR" << RESET << "] " << logMessage << endl;
		msg.msg = "(i)[ERROR] " + logMessage;
		msg.color[0] = 1.0f;
		msg.color[1] = 0;
		msg.color[2] = 0;
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::WARN_NORMAL:
		cout << "(" << BRIGHT_YELLOW << "i" << RESET << ")[" << BRIGHT_YELLOW << "WARN" << RESET << "] " << logMessage << endl;
		msg.msg = "(i)[WARN] " + logMessage;
		msg.color[0] = 1.0f;
		msg.color[1] = 1.0f;
		msg.color[2] = 0;
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::WARN_CAN_CAUSE_ERROR:
		cout << "(" << YELLOW << "!" << RESET << ")[" << YELLOW << "WARN" << RESET << "] " << logMessage << endl;
		msg.msg = "(!)[WARN] " + logMessage;
		msg.color[0] = 1.0f;
		msg.color[1] = 0.5f;
		msg.color[2] = 0;
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::INFO_NORMAL:
		cout << "(" << GREEN << "i" << RESET << ")[" << GREEN << "INFO" << RESET << "] " << logMessage << endl;
		msg.msg = "(i)[INFO] " + logMessage;
		msg.color[0] = 0;
		msg.color[1] = 1.0f;
		msg.color[2] = 0;
		EngineConsole::messages.push_back(msg);
		break;
	case UreTechEngine::EngineConsole::DEBUG:
		cout << "(" << MAGENTA << "D" << RESET << ")[" << MAGENTA << "DEBUG" << RESET << "] " << logMessage << endl;
		msg.msg = "(D)[DEBUG] " + logMessage;
		msg.color[0] = 1.0f;
		msg.color[1] = 0;
		msg.color[2] = 0.56f;
		EngineConsole::messages.push_back(msg);
		break;
	default:
		cout << BRIGHT_BLUE << "UNKNOWN: an unknown error reported! check your code." << RESET << endl;
		msg.msg = "UNKNOWN: an unknown error reported! check your code.";
		msg.color[0] = 0;
		msg.color[1] = 0;
		msg.color[2] = 1.0f;
		EngineConsole::messages.push_back(msg);
		break;
	}
}

void UreTechEngine::EngineConsole::forceToExit()
{
	cout << "press esc to exit..." << endl;
	while (getch()!= 27) {}
	exit(-1);
}
