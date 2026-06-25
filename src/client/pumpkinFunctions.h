/*
*
* Function Declarations Header
* Built 2026-06-24 09:10 PM
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
typedef int (APIENTRYP FPPUMPKIN_LOADFILE)(std::string filePath, bool pack, bool cache, bool relative, bool binary);
typedef void (APIENTRYP FPPUMPKIN_FORGETFILE)(std::string const& path);
typedef FileData (APIENTRYP FPPUMPKIN_READFILE)(std::string const& path, bool binary);
typedef std::string (APIENTRYP FPPUMPKIN_TORELATIVEPATH)(std::string const& path);
typedef bool (APIENTRYP FPPUMPKIN_OPENFILEFUNC)(std::string const& location, bool relative, bool binary, ::pPack::FileHandle& handle);
typedef void (APIENTRYP FPPUMPKIN_CLOSEFILEFUNC)(::pPack::FileHandle& handle);


// **************************************************
// Declarations

APIGET FPPUMPKIN_INIT Pumpkin_Init;
APIGET FPPUMPKIN_UPDATE Pumpkin_Update;
APIGET FPPUMPKIN_END Pumpkin_End;
APIGET FPPUMPKIN_GETRUNTIME Pumpkin_GetRuntime;
APIGET FPPUMPKIN_EXECUTABLELOCATION Pumpkin_ExecutableLocation;
APIGET FPPUMPKIN_PRINTERROR Pumpkin_PrintError;
APIGET FPPUMPKIN_DELTATIME Pumpkin_DeltaTime;
APIGET FPPUMPKIN_TOTALTIME Pumpkin_TotalTime;
APIGET FPPUMPKIN_REGISTEROBJECT Pumpkin_RegisterObject;
APIGET FPPUMPKIN_GETOBJECT Pumpkin_GetObject;
APIGET FPPUMPKIN_DELETEOBJECT Pumpkin_DeleteObject;
APIGET FPOBJECT_GETNAME Object_GetName;
APIGET FPOBJECT_SETMODEL Object_SetModel;
APIGET FPOBJECT_ADDSCRIPT Object_AddScript;
APIGET FPOBJECT_GETSCRIPT Object_GetScript;
APIGET FPOBJECT_REMOVESCRIPT Object_RemoveScript;
APIGET FPOBJECT_ADDDELETECALLBACK Object_AddDeleteCallback;
APIGET FPOBJECT_REMOVEDELETECALLBACK Object_RemoveDeleteCallback;
APIGET FPTRANSFORM_GENERATEMODEL Transform_GenerateModel;
APIGET FPPUMPKIN_REGISTERCAMERA Pumpkin_RegisterCamera;
APIGET FPPUMPKIN_GETCAMERA Pumpkin_GetCamera;
APIGET FPPUMPKIN_SETPRIMARYCAMERA Pumpkin_SetPrimaryCamera;
APIGET FPPUMPKIN_GETPRIMARYCAMERA Pumpkin_GetPrimaryCamera;
APIGET FPCAMERA_GENERATEVIEW Camera_GenerateView;
APIGET FPCAMERA_GENERATEPROJECTION Camera_GenerateProjection;
APIGET FPCAMERA_FORWARD Camera_Forward;
APIGET FPCAMERA_RIGHT Camera_Right;
APIGET FPCAMERA_GETANGLEBASED Camera_GetAngleBased;
APIGET FPCAMERA_ANGLEBASED Camera_AngleBased;
APIGET FPCAMERA_LOOKATTARGET Camera_LookAtTarget;
APIGET FPCAMERA_GETLOOKATTARGET Camera_GetLookAtTarget;
APIGET FPPUMPKIN_REGISTERMESH Pumpkin_RegisterMesh;
APIGET FPPUMPKIN_GETMESH Pumpkin_GetMesh;
APIGET FPPUMPKIN_REGISTERFORMAT Pumpkin_RegisterFormat;
APIGET FPPUMPKIN_GETFORMAT Pumpkin_GetFormat;
APIGET FPPUMPKIN_APPLYSTATICBUFFER Pumpkin_ApplyStaticBuffer;
APIGET FPPUMPKIN_REGISTERMODEL Pumpkin_RegisterModel;
APIGET FPPUMPKIN_GETMODEL Pumpkin_GetModel;
APIGET FPMODEL_SETSHADER Model_SetShader;
APIGET FPMODEL_SETMESH Model_SetMesh;
APIGET FPMODEL_GETPROPERTIES Model_GetProperties;
APIGET FPPUMPKIN_REGISTERSHADER Pumpkin_RegisterShader;
APIGET FPPUMPKIN_GETSHADER Pumpkin_GetShader;
APIGET FPSHADER_GETPROPERTIES Shader_GetProperties;
APIGET FPPROPERTYHOLDER_ADDPROPERTY PropertyHolder_AddProperty;
APIGET FPPROPERTYHOLDER_SETPROPERTY PropertyHolder_SetProperty;
APIGET FPPROPERTYHOLDER_SETORADDPROPERTY PropertyHolder_SetOrAddProperty;
APIGET FPPROPERTYHOLDER_DELETEPROPERTY PropertyHolder_DeleteProperty;
APIGET FPPROPERTYHOLDER_GETPROPERTY PropertyHolder_GetProperty;
APIGET FPPUMPKIN_REGISTERSCRIPTRAW Pumpkin_RegisterScriptRaw;
APIGET FPPUMPKIN_CREATESCRIPT Pumpkin_CreateScript;
APIGET FPPUMPKIN_GETSCRIPTNAME Pumpkin_GetScriptName;
APIGET FPPUMPKIN_LOADFILE Pumpkin_LoadFile;
APIGET FPPUMPKIN_FORGETFILE Pumpkin_ForgetFile;
APIGET FPPUMPKIN_READFILE Pumpkin_ReadFile;
APIGET FPPUMPKIN_TORELATIVEPATH Pumpkin_ToRelativePath;
APIGET FPPUMPKIN_OPENFILEFUNC Pumpkin_OpenFileFunc;
APIGET FPPUMPKIN_CLOSEFILEFUNC Pumpkin_CloseFileFunc;

bool LoadFunctions(PROCADDRESSFUNC proc);

}
#endif