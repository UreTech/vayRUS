#ifdef GMODULE_APP
#define ENGINE_DEFINE __declspec(dllimport)
#define GAME_DEFINE __declspec(dllexport)
#elif GMODULE_ENGINE_DLL
#define ENGINE_DEFINE __declspec(dllexport)
#define GAME_DEFINE __declspec(dllimport) // this should not be used anywhere
#else // GMODULE_DLL
#error GMODULE TYPE MUST BE DEFINED
#endif

#define DLL_EXPORT_AS(func, exportName) \
__pragma(comment(linker, "/EXPORT:" #exportName "=" #func))

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

class ENGINE_DEFINE gmodule_builder {
public:
	static bool build_gmodule(const char* module_vcxproj_path);
};

#endif// GMODULE_H