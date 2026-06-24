/*
*
* Function Declarations Header
* Built 2026-06-24 09:32 AM
*
*/

#ifndef __FTFP__BUILD__H

// Windows specific for the moment
#define APIENTRY __stdcall
#define APIENTRYP APIENTRY *
#define APIGET extern
typedef void* (*PROCADDRESSFUNC)(char const* addr);

#include "pumpkin/types.h"
namespace pumpkin {

typedef StartReturn (APIENTRYP FPPUMPKIN_INIT)(StartSettings const& start, int argv, char** argc, void (*devLoad)());
typedef void (APIENTRYP FPPUMPKIN_UPDATE)();
typedef void (APIENTRYP FPPUMPKIN_END)();
typedef RuntimeSettings* (APIENTRYP FPPUMPKIN_GETRUNTIME)();
typedef std::string (APIENTRYP FPPUMPKIN_EXECUTABLELOCATION)();
typedef void (APIENTRYP FPPUMPKIN_PRINTERROR)(PrintLevel level, char const* file, char const* msg);
typedef double (APIENTRYP FPPUMPKIN_DELTATIME)();
typedef double (APIENTRYP FPPUMPKIN_TOTALTIME)();
typedef Object* (APIENTRYP FPPUMPKIN_REGISTEROBJECT)(std::string const& name);
typedef Object* (APIENTRYP FPPUMPKIN_GETOBJECT)(std::string const& name);
typedef bool (APIENTRYP FPPUMPKIN_DELETEOBJECT)(std::string const& name);
typedef char const* (APIENTRYP FPOBJECT_GETNAME)(Object const* object);
typedef bool (APIENTRYP FPOBJECT_SETMODEL)(Object* object, Model* model);
typedef bool (APIENTRYP FPOBJECT_ADDSCRIPT)(Object* object, std::string const& name);
typedef Script* (APIENTRYP FPOBJECT_GETSCRIPT)(Object* object, std::string const& name);
typedef bool (APIENTRYP FPOBJECT_REMOVESCRIPT)(Object* object, std::string const& name);
typedef void (APIENTRYP FPOBJECT_ADDDELETECALLBACK)(Object* object, ObjectDeleteCallback ptrFunc, int id);
typedef void (APIENTRYP FPOBJECT_REMOVEDELETECALLBACK)(Object* object, ObjectDeleteCallback ptrFunc, int id);
typedef void (APIENTRYP FPTRANSFORM_GENERATEMODEL)(Transform transform, MatrixWrapper& store);
typedef Camera* (APIENTRYP FPPUMPKIN_REGISTERCAMERA)(std::string const& name);
typedef Camera* (APIENTRYP FPPUMPKIN_GETCAMERA)(std::string const& name);
typedef bool (APIENTRYP FPPUMPKIN_SETPRIMARYCAMERA)(Camera* camera);
typedef Camera* (APIENTRYP FPPUMPKIN_GETPRIMARYCAMERA)();
typedef void (APIENTRYP FPCAMERA_GENERATEVIEW)(Camera* camera);
typedef void (APIENTRYP FPCAMERA_GENERATEPROJECTION)(Camera* camera);
typedef ::pPack::Vector3* (APIENTRYP FPCAMERA_FORWARD)(Camera* camera);
typedef ::pPack::Vector3* (APIENTRYP FPCAMERA_RIGHT)(Camera* camera);
typedef bool (APIENTRYP FPCAMERA_GETANGLEBASED)(Camera* camera);
typedef void (APIENTRYP FPCAMERA_ANGLEBASED)(Camera* camera, bool b);
typedef void (APIENTRYP FPCAMERA_LOOKATTARGET)(Camera* camera, ::pPack::Vector3* target);
typedef ::pPack::Vector3* (APIENTRYP FPCAMERA_GETLOOKATTARGET)(Camera* camera);
typedef Mesh* (APIENTRYP FPPUMPKIN_REGISTERMESH)(std::string const& name, void* vertices, size_t size, size_t count, bool dynamic, GLuint format);
typedef Mesh* (APIENTRYP FPPUMPKIN_GETMESH)(std::string const& name);
typedef GLuint (APIENTRYP FPPUMPKIN_REGISTERFORMAT)(std::string const& name, FormatStartInfo const* const formatStartInfo, GLuint count, bool autoOffset);
typedef GLuint (APIENTRYP FPPUMPKIN_GETFORMAT)(std::string const& name);
typedef void (APIENTRYP FPPUMPKIN_APPLYSTATICBUFFER)();
typedef Model* (APIENTRYP FPPUMPKIN_REGISTERMODEL)(std::string const& name);
typedef Model* (APIENTRYP FPPUMPKIN_GETMODEL)(std::string const& name);
typedef bool (APIENTRYP FPMODEL_SETSHADER)(Model* model, Shader* shader);
typedef bool (APIENTRYP FPMODEL_SETMESH)(Model* model, Mesh* mesh);
typedef PropertyHolder* (APIENTRYP FPMODEL_GETPROPERTIES)(Model* shader);
typedef Shader* (APIENTRYP FPPUMPKIN_REGISTERSHADER)(std::string const& name, ShaderInfo* startInfos, int count);
typedef Shader* (APIENTRYP FPPUMPKIN_GETSHADER)(std::string const& name);
typedef PropertyHolder* (APIENTRYP FPSHADER_GETPROPERTIES)(Shader* shader);
typedef bool (APIENTRYP FPPROPERTYHOLDER_ADDPROPERTY)(PropertyHolder* holder, std::string const& name, void* value, VariableType type);
typedef bool (APIENTRYP FPPROPERTYHOLDER_SETPROPERTY)(PropertyHolder* holder, std::string const& name, void* value);
typedef bool (APIENTRYP FPPROPERTYHOLDER_SETORADDPROPERTY)(PropertyHolder* holder, std::string const& name, void* value, VariableType type);
typedef void (APIENTRYP FPPROPERTYHOLDER_DELETEPROPERTY)(PropertyHolder* holder, std::string const& name);
typedef void* (APIENTRYP FPPROPERTYHOLDER_GETPROPERTY)(PropertyHolder* holder, std::string const& name);
typedef bool (APIENTRYP FPPUMPKIN_REGISTERSCRIPTRAW)(ScriptAllocateFunction scriptAllocate, std::string const& name, size_t size);
typedef Script* (APIENTRYP FPPUMPKIN_CREATESCRIPT)(std::string const& name);
typedef char const* (APIENTRYP FPPUMPKIN_GETSCRIPTNAME)(Script* script);


// **************************************************
// Declarations

APIGET FPPUMPKIN_INIT Pumpkin_Init = NULL;
APIGET FPPUMPKIN_UPDATE Pumpkin_Update = NULL;
APIGET FPPUMPKIN_END Pumpkin_End = NULL;
APIGET FPPUMPKIN_GETRUNTIME Pumpkin_GetRuntime = NULL;
APIGET FPPUMPKIN_EXECUTABLELOCATION Pumpkin_ExecutableLocation = NULL;
APIGET FPPUMPKIN_PRINTERROR Pumpkin_PrintError = NULL;
APIGET FPPUMPKIN_DELTATIME Pumpkin_DeltaTime = NULL;
APIGET FPPUMPKIN_TOTALTIME Pumpkin_TotalTime = NULL;
APIGET FPPUMPKIN_REGISTEROBJECT Pumpkin_RegisterObject = NULL;
APIGET FPPUMPKIN_GETOBJECT Pumpkin_GetObject = NULL;
APIGET FPPUMPKIN_DELETEOBJECT Pumpkin_DeleteObject = NULL;
APIGET FPOBJECT_GETNAME Object_GetName = NULL;
APIGET FPOBJECT_SETMODEL Object_SetModel = NULL;
APIGET FPOBJECT_ADDSCRIPT Object_AddScript = NULL;
APIGET FPOBJECT_GETSCRIPT Object_GetScript = NULL;
APIGET FPOBJECT_REMOVESCRIPT Object_RemoveScript = NULL;
APIGET FPOBJECT_ADDDELETECALLBACK Object_AddDeleteCallback = NULL;
APIGET FPOBJECT_REMOVEDELETECALLBACK Object_RemoveDeleteCallback = NULL;
APIGET FPTRANSFORM_GENERATEMODEL Transform_GenerateModel = NULL;
APIGET FPPUMPKIN_REGISTERCAMERA Pumpkin_RegisterCamera = NULL;
APIGET FPPUMPKIN_GETCAMERA Pumpkin_GetCamera = NULL;
APIGET FPPUMPKIN_SETPRIMARYCAMERA Pumpkin_SetPrimaryCamera = NULL;
APIGET FPPUMPKIN_GETPRIMARYCAMERA Pumpkin_GetPrimaryCamera = NULL;
APIGET FPCAMERA_GENERATEVIEW Camera_GenerateView = NULL;
APIGET FPCAMERA_GENERATEPROJECTION Camera_GenerateProjection = NULL;
APIGET FPCAMERA_FORWARD Camera_Forward = NULL;
APIGET FPCAMERA_RIGHT Camera_Right = NULL;
APIGET FPCAMERA_GETANGLEBASED Camera_GetAngleBased = NULL;
APIGET FPCAMERA_ANGLEBASED Camera_AngleBased = NULL;
APIGET FPCAMERA_LOOKATTARGET Camera_LookAtTarget = NULL;
APIGET FPCAMERA_GETLOOKATTARGET Camera_GetLookAtTarget = NULL;
APIGET FPPUMPKIN_REGISTERMESH Pumpkin_RegisterMesh = NULL;
APIGET FPPUMPKIN_GETMESH Pumpkin_GetMesh = NULL;
APIGET FPPUMPKIN_REGISTERFORMAT Pumpkin_RegisterFormat = NULL;
APIGET FPPUMPKIN_GETFORMAT Pumpkin_GetFormat = NULL;
APIGET FPPUMPKIN_APPLYSTATICBUFFER Pumpkin_ApplyStaticBuffer = NULL;
APIGET FPPUMPKIN_REGISTERMODEL Pumpkin_RegisterModel = NULL;
APIGET FPPUMPKIN_GETMODEL Pumpkin_GetModel = NULL;
APIGET FPMODEL_SETSHADER Model_SetShader = NULL;
APIGET FPMODEL_SETMESH Model_SetMesh = NULL;
APIGET FPMODEL_GETPROPERTIES Model_GetProperties = NULL;
APIGET FPPUMPKIN_REGISTERSHADER Pumpkin_RegisterShader = NULL;
APIGET FPPUMPKIN_GETSHADER Pumpkin_GetShader = NULL;
APIGET FPSHADER_GETPROPERTIES Shader_GetProperties = NULL;
APIGET FPPROPERTYHOLDER_ADDPROPERTY PropertyHolder_AddProperty = NULL;
APIGET FPPROPERTYHOLDER_SETPROPERTY PropertyHolder_SetProperty = NULL;
APIGET FPPROPERTYHOLDER_SETORADDPROPERTY PropertyHolder_SetOrAddProperty = NULL;
APIGET FPPROPERTYHOLDER_DELETEPROPERTY PropertyHolder_DeleteProperty = NULL;
APIGET FPPROPERTYHOLDER_GETPROPERTY PropertyHolder_GetProperty = NULL;
APIGET FPPUMPKIN_REGISTERSCRIPTRAW Pumpkin_RegisterScriptRaw = NULL;
APIGET FPPUMPKIN_CREATESCRIPT Pumpkin_CreateScript = NULL;
APIGET FPPUMPKIN_GETSCRIPTNAME Pumpkin_GetScriptName = NULL;

bool LoadFunctions(PROCADDRESSFUNC proc);

}
#endif