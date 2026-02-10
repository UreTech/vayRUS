#define GMODULE_APP
#include<UreTechEngine/gmodule/gmodule.h>

#define MODULE_EXPORT extern "C"

#define  MODULE_BEGIN_FUNC(func) DLL_EXPORT_AS(func,vayRUS3D_GMODULE_BEGIN_FUNC_V0)
#define  MODULE_DESTROY_FUNC(func) DLL_EXPORT_AS(func,vayRUS3D_GMODULE_DESTROY_FUNC_V0)

MODULE_BEGIN_FUNC(module_begin);
MODULE_DESTROY_FUNC(module_destroy);
