/*
*
* Function Declarations Header
* Built 2026-06-23 11:16 PM
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

typedef StartReturn (APIENTRYP FPINIT)(StartSettings const& start, int argv, char** argc, void (*devLoad)());
typedef void (APIENTRYP FPUPDATE)();
typedef void (APIENTRYP FPEND)();
typedef RuntimeSettings* (APIENTRYP FPGETRUNTIME)();
typedef std::string (APIENTRYP FPEXECUTABLELOCATION)();
typedef void (APIENTRYP FPPRINTERROR)(PrintLevel level, char const* file, char const* msg);
typedef double (APIENTRYP FPDELTATIME)();
typedef Object* (APIENTRYP FPREGISTEROBJECT)(std::string const& name);
typedef Object* (APIENTRYP FPGETOBJECT)(std::string const& name);
typedef bool (APIENTRYP FPDELETEOBJECT)(std::string const& name);
typedef char const* (APIENTRYP FPOBJECT_GETNAME)(Object const* object);
typedef bool (APIENTRYP FPOBJECT_SETMODEL)(Object* object, Model* model);
typedef bool (APIENTRYP FPOBJECT_ADDSCRIPT)(Object* object, std::string const& name);
typedef Script* (APIENTRYP FPOBJECT_GETSCRIPT)(Object* object, std::string const& name);
typedef bool (APIENTRYP FPOBJECT_REMOVESCRIPT)(Object* object, std::string const& name);
typedef void (APIENTRYP FPOBJECT_ADDDELETECALLBACK)(Object* object, ObjectDeleteCallback ptrFunc, int id);
typedef void (APIENTRYP FPOBJECT_REMOVEDELETECALLBACK)(Object* object, ObjectDeleteCallback ptrFunc, int id);
typedef void (APIENTRYP FPTRANSFORM_GENERATEMODEL)(Transform transform, MatrixWrapper& store);
typedef Camera* (APIENTRYP FPREGISTERCAMERA)(std::string const& name);
typedef Camera* (APIENTRYP FPGETCAMERA)(std::string const& name);
typedef bool (APIENTRYP FPSETPRIMARYCAMERA)(Camera* camera);
typedef Camera* (APIENTRYP FPGETPRIMARYCAMERA)();
typedef void (APIENTRYP FPCAMERA_GENERATEVIEW)(Camera* camera);
typedef void (APIENTRYP FPCAMERA_GENERATEPROJECTION)(Camera* camera);
typedef ::pPack::Vector3* (APIENTRYP FPCAMERA_FORWARD)(Camera* camera);
typedef ::pPack::Vector3* (APIENTRYP FPCAMERA_RIGHT)(Camera* camera);
typedef bool (APIENTRYP FPCAMERA_GETANGLEBASED)(Camera* camera);
typedef void (APIENTRYP FPCAMERA_ANGLEBASED)(Camera* camera, bool b);
typedef void (APIENTRYP FPCAMERA_LOOKATTARGET)(Camera* camera, ::pPack::Vector3* target);
typedef ::pPack::Vector3* (APIENTRYP FPCAMERA_GETLOOKATTARGET)(Camera* camera);
typedef Mesh* (APIENTRYP FPREGISTERMESH)(std::string const& name, void* vertices, size_t size, size_t count, bool dynamic, GLuint format);
typedef Mesh* (APIENTRYP FPGETMESH)(std::string const& name);
typedef GLuint (APIENTRYP FPREGISTERFORMAT)(std::string const& name, FormatStartInfo const* const formatStartInfo, GLuint count, bool autoOffset);
typedef GLuint (APIENTRYP FPGETFORMAT)(std::string const& name);
typedef void (APIENTRYP FPAPPLYSTATICBUFFER)();
typedef Model* (APIENTRYP FPREGISTERMODEL)(std::string const& name);
typedef Model* (APIENTRYP FPGETMODEL)(std::string const& name);
typedef bool (APIENTRYP FPMODEL_SETSHADER)(Model* model, Shader* shader);
typedef bool (APIENTRYP FPMODEL_SETMESH)(Model* model, Mesh* mesh);
typedef PropertyHolder* (APIENTRYP FPMODEL_GETPROPERTIES)(Model* shader);
typedef Shader* (APIENTRYP FPREGISTERSHADER)(std::string const& name, ShaderInfo* startInfos, int count);
typedef Shader* (APIENTRYP FPGETSHADER)(std::string const& name);
typedef PropertyHolder* (APIENTRYP FPSHADER_GETPROPERTIES)(Shader* shader);
typedef bool (APIENTRYP FPPROPERTYHOLDER_ADDPROPERTY)(PropertyHolder* holder, std::string const& name, void* value, VariableType type);
typedef bool (APIENTRYP FPPROPERTYHOLDER_SETPROPERTY)(PropertyHolder* holder, std::string const& name, void* value);
typedef bool (APIENTRYP FPPROPERTYHOLDER_SETORADDPROPERTY)(PropertyHolder* holder, std::string const& name, void* value, VariableType type);
typedef void (APIENTRYP FPPROPERTYHOLDER_DELETEPROPERTY)(PropertyHolder* holder, std::string const& name);
typedef void* (APIENTRYP FPPROPERTYHOLDER_GETPROPERTY)(PropertyHolder* holder, std::string const& name);
typedef bool (APIENTRYP FPREGISTERSCRIPTRAW)(ScriptAllocateFunction scriptAllocate, std::string const& name, size_t size);
typedef Script* (APIENTRYP FPCREATESCRIPT)(std::string const& name);
typedef char const* (APIENTRYP FPGETSCRIPTNAME)(Script* script);


// **************************************************
// Declarations

APIGET FPINIT Init = NULL;
APIGET FPUPDATE Update = NULL;
APIGET FPEND End = NULL;
APIGET FPGETRUNTIME GetRuntime = NULL;
APIGET FPEXECUTABLELOCATION ExecutableLocation = NULL;
APIGET FPPRINTERROR PrintError = NULL;
APIGET FPDELTATIME DeltaTime = NULL;
APIGET FPREGISTEROBJECT RegisterObject = NULL;
APIGET FPGETOBJECT GetObject = NULL;
APIGET FPDELETEOBJECT DeleteObject = NULL;
APIGET FPOBJECT_GETNAME Object_GetName = NULL;
APIGET FPOBJECT_SETMODEL Object_SetModel = NULL;
APIGET FPOBJECT_ADDSCRIPT Object_AddScript = NULL;
APIGET FPOBJECT_GETSCRIPT Object_GetScript = NULL;
APIGET FPOBJECT_REMOVESCRIPT Object_RemoveScript = NULL;
APIGET FPOBJECT_ADDDELETECALLBACK Object_AddDeleteCallback = NULL;
APIGET FPOBJECT_REMOVEDELETECALLBACK Object_RemoveDeleteCallback = NULL;
APIGET FPTRANSFORM_GENERATEMODEL Transform_GenerateModel = NULL;
APIGET FPREGISTERCAMERA RegisterCamera = NULL;
APIGET FPGETCAMERA GetCamera = NULL;
APIGET FPSETPRIMARYCAMERA SetPrimaryCamera = NULL;
APIGET FPGETPRIMARYCAMERA GetPrimaryCamera = NULL;
APIGET FPCAMERA_GENERATEVIEW Camera_GenerateView = NULL;
APIGET FPCAMERA_GENERATEPROJECTION Camera_GenerateProjection = NULL;
APIGET FPCAMERA_FORWARD Camera_Forward = NULL;
APIGET FPCAMERA_RIGHT Camera_Right = NULL;
APIGET FPCAMERA_GETANGLEBASED Camera_GetAngleBased = NULL;
APIGET FPCAMERA_ANGLEBASED Camera_AngleBased = NULL;
APIGET FPCAMERA_LOOKATTARGET Camera_LookAtTarget = NULL;
APIGET FPCAMERA_GETLOOKATTARGET Camera_GetLookAtTarget = NULL;
APIGET FPREGISTERMESH RegisterMesh = NULL;
APIGET FPGETMESH GetMesh = NULL;
APIGET FPREGISTERFORMAT RegisterFormat = NULL;
APIGET FPGETFORMAT GetFormat = NULL;
APIGET FPAPPLYSTATICBUFFER ApplyStaticBuffer = NULL;
APIGET FPREGISTERMODEL RegisterModel = NULL;
APIGET FPGETMODEL GetModel = NULL;
APIGET FPMODEL_SETSHADER Model_SetShader = NULL;
APIGET FPMODEL_SETMESH Model_SetMesh = NULL;
APIGET FPMODEL_GETPROPERTIES Model_GetProperties = NULL;
APIGET FPREGISTERSHADER RegisterShader = NULL;
APIGET FPGETSHADER GetShader = NULL;
APIGET FPSHADER_GETPROPERTIES Shader_GetProperties = NULL;
APIGET FPPROPERTYHOLDER_ADDPROPERTY PropertyHolder_AddProperty = NULL;
APIGET FPPROPERTYHOLDER_SETPROPERTY PropertyHolder_SetProperty = NULL;
APIGET FPPROPERTYHOLDER_SETORADDPROPERTY PropertyHolder_SetOrAddProperty = NULL;
APIGET FPPROPERTYHOLDER_DELETEPROPERTY PropertyHolder_DeleteProperty = NULL;
APIGET FPPROPERTYHOLDER_GETPROPERTY PropertyHolder_GetProperty = NULL;
APIGET FPREGISTERSCRIPTRAW RegisterScriptRaw = NULL;
APIGET FPCREATESCRIPT CreateScript = NULL;
APIGET FPGETSCRIPTNAME GetScriptName = NULL;

bool LoadFunctions(PROCADDRESSFUNC proc);

}
#endif