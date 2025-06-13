#ifdef GMODULE_APP
#define ENGINE_DEFINE __declspec(dllimport)
#elif GMODULE_ENGINE_DLL
#define ENGINE_DEFINE __declspec(dllexport)
#else // GMODULE_DLL
//#error dll define
#define ENGINE_DEFINE __declspec(dllimport)
#endif

#ifndef GMODULE_H
#define GMODULE_H
#include<UreTechEngine/utils/string/string.h>
#include <windows.h>

struct gmodule {
	HMODULE moduleHandle;
};

class ENGINE_DEFINE gmodule_loader {
public:
	static gmodule load_gmodule(const char* modulePath);
};

#endif// GMODULE_H