/*
*
* Function Declarations Header
* Built 2026-08-31 04:04 PM
*
*/

#ifndef __FTFP__BUILD__H

// Windows specific for the moment
#define APIENTRY __stdcall
#define APIENTRYP APIENTRY *
#define APIGET extern
typedef void* (*PROCADDRESSFUNC)(char const* addr);

#include "pumpkin/types.h"
#include <vector>
namespace pumpkin {

typedef StartReturn (APIENTRYP FPPUMPKIN_INIT)(StartSettings const& start, int argc, char** argv, void (*devLoad)());
typedef void (APIENTRYP FPPUMPKIN_UPDATE)();
typedef void (APIENTRYP FPPUMPKIN_END)();
typedef RuntimeSettings* (APIENTRYP FPPUMPKIN_GETRUNTIME)();
typedef std::string (APIENTRYP FPPUMPKIN_EXECUTABLELOCATION)();
typedef void (APIENTRYP FPPUMPKIN_PRINTERROR)(PrintLevel level, char const* file, char const* msg);
typedef double* (APIENTRYP FPPUMPKIN_DELTATIME)();
typedef double* (APIENTRYP FPPUMPKIN_TOTALTIME)();
typedef void (APIENTRYP FPPUMPKIN_STARTMEMORYIGNOREBLOCK)();
typedef void (APIENTRYP FPPUMPKIN_ENDMEMORYIGNOREBLOCK)();
typedef ExplodedObjectList (APIENTRYP FPPUMPKIN_EXPLODEALLOBJECTS)();
typedef bool (APIENTRYP FPPUMPKIN_CASTRAY)(Ray const& ray, RayHitInfo& hit);
typedef bool (APIENTRYP FPPUMPKIN_CASTRAYINTO)(Ray const& ray, RayHitInfo& hit, ExplodedObjectList const& list);
typedef ExplodedObjectList (APIENTRYP FPPUMPKIN_WRAPBVHNODESAROUND)(ExplodedObjectList const& list);
typedef ::pPack::DVector3 (APIENTRYP FPRAY_AT)(Ray const& ray, double time);
typedef double (APIENTRYP FPINTERVAL_SIZE)(Interval const& interval);
typedef void (APIENTRYP FPINTERVAL_EXPAND)(Interval& interval, double delta);
typedef void (APIENTRYP FPINTERVAL_OFFSET)(Interval& interval, double offset);
typedef bool (APIENTRYP FPINTERVAL_CONTAINS)(Interval const& interval, double value);
typedef double (APIENTRYP FPINTERVAL_CLAMP)(Interval const& interval, double value);
typedef Interval (APIENTRYP FPINTERVAL_UNION)(Interval const& a, Interval const& b);
typedef AABB (APIENTRYP FPAABB_GENERATEFROMOBJECTS)(std::vector<CollisionObject*> const& objects, size_t start, size_t end);
typedef AABB (APIENTRYP FPAABB_GENERATEFROMPOINTS)(::pPack::DVector3 const& a, ::pPack::DVector3 const& b);
typedef void (APIENTRYP FPAABB_PADTOMINIMUMS)(AABB& aabb);
typedef int (APIENTRYP FPAABB_LONGESTAXIS)(AABB const& aabb);
typedef Interval (APIENTRYP FPCONST&AABB_AXISINTERVAL)(AABB const& aabb, int axis);
typedef void (APIENTRYP FPAABB_OFFSET)(AABB& aabb, ::pPack::DVector3 offset);
typedef AABB (APIENTRYP FPAABB_COMBINE)(AABB const& a, AABB const& b);
typedef void (APIENTRYP FPAABB_EXPAND)(AABB& aabb, double delta);
typedef bool (APIENTRYP FPAABB_COLLIDESWITH)(AABB const& aabb, Ray const& ray, Interval interval);
typedef void (APIENTRYP FPCOLLISIONTEMPLATEPLANE_CONSTRUCT)(CollisionTemplatePlane& plane);
typedef void (APIENTRYP FPCOLLISIONTEMPLATEPLANE_MOVEINTOCOORDINATESPACE)(CollisionTemplatePlane const& plane, ::pPack::DVector3 point, ::pPack::DVector2& out);
typedef void (APIENTRYP FPEXPLODEDOBJECTLIST_DELETEALL)(ExplodedObjectList& list);
typedef Object* (APIENTRYP FPPUMPKIN_REGISTEROBJECT)(std::string const& name);
typedef Object* (APIENTRYP FPPUMPKIN_GETOBJECT)(std::string const& name);
typedef bool (APIENTRYP FPPUMPKIN_DELETEOBJECT)(std::string const& name);
typedef char const* (APIENTRYP FPOBJECT_GETNAME)(Object const* object);
typedef Model* (APIENTRYP FPOBJECT_SETMODEL)(Object* object, Model* model);
typedef Model* (APIENTRYP FPOBJECT_GETMODEL)(Object* object);
typedef Script* (APIENTRYP FPOBJECT_ADDSCRIPT)(Object* object, std::string const& name);
typedef Script* (APIENTRYP FPOBJECT_GETSCRIPT)(Object* object, std::string const& name);
typedef bool (APIENTRYP FPOBJECT_REMOVESCRIPT)(Object* object, std::string const& name);
typedef std::vector<Script*>&& (APIENTRYP FPOBJECT_GETALLSCRIPTS)(Object* object);
typedef void (APIENTRYP FPOBJECT_ADDDELETECALLBACK)(Object* object, ObjectDeleteCallback ptrFunc, int id);
typedef void (APIENTRYP FPOBJECT_REMOVEDELETECALLBACK)(Object* object, ObjectDeleteCallback ptrFunc, int id);
typedef Object* (APIENTRYP FPOBJECT_DUPLICATE)(Object* object, std::string const& name);
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
typedef Format (APIENTRYP FPPUMPKIN_REGISTERFORMAT)(std::string const& name, FormatStartInfo* formatStartInfo, GLuint count, bool autoOffset);
typedef Format (APIENTRYP FPPUMPKIN_GETFORMAT)(std::string const& name);
typedef FormatStartInfo (APIENTRYP FPCONST*CONSTFORMAT_GETATTRIBUTEOFNAME)(Format const& format, AttributeName name);
typedef Mesh* (APIENTRYP FPPUMPKIN_REGISTERMESH)(std::string const& name, void* vertices, size_t size, size_t count, bool dynamic, Format format);
typedef Mesh* (APIENTRYP FPPUMPKIN_GETMESH)(std::string const& name);
typedef void (APIENTRYP FPPUMPKIN_APPLYSTATICBUFFER)();
typedef MeshInfo (APIENTRYP FPMESH_GETINFO)(Mesh const*const mesh);
typedef void (APIENTRYP FPMESH_RELOAD)(Mesh* mesh);
typedef std::string (APIENTRYP FPMESH_GETNAME)(Mesh* mesh);
typedef Mesh* (APIENTRYP FPMESH_DUPLICATEASDYNAMIC)(Mesh* mesh, std::string const& name);
typedef Model* (APIENTRYP FPPUMPKIN_REGISTERMODEL)(std::string const& name, void(*setup)());
typedef Model* (APIENTRYP FPPUMPKIN_GETMODEL)(std::string const& name);
typedef bool (APIENTRYP FPMODEL_SETSHADER)(Model* model, Shader* shader);
typedef bool (APIENTRYP FPMODEL_SETMESH)(Model* model, Mesh* mesh);
typedef Shader* (APIENTRYP FPMODEL_GETSHADER)(Model* model);
typedef Mesh* (APIENTRYP FPMODEL_GETMESH)(Model* model);
typedef std::string (APIENTRYP FPMODEL_GETNAME)(Model* model);
typedef PropertyHolder* (APIENTRYP FPMODEL_GETPROPERTIES)(Model* shader);
typedef Shader* (APIENTRYP FPPUMPKIN_REGISTERSHADER)(std::string const& name, ShaderInfo* startInfos, int count, void(*setup)());
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
APIGET FPPUMPKIN_STARTMEMORYIGNOREBLOCK Pumpkin_StartMemoryIgnoreBlock;
APIGET FPPUMPKIN_ENDMEMORYIGNOREBLOCK Pumpkin_EndMemoryIgnoreBlock;
APIGET FPPUMPKIN_EXPLODEALLOBJECTS Pumpkin_ExplodeAllObjects;
APIGET FPPUMPKIN_CASTRAY Pumpkin_CastRay;
APIGET FPPUMPKIN_CASTRAYINTO Pumpkin_CastRayInto;
APIGET FPPUMPKIN_WRAPBVHNODESAROUND Pumpkin_WrapBVHNodesAround;
APIGET FPRAY_AT Ray_At;
APIGET FPINTERVAL_SIZE Interval_Size;
APIGET FPINTERVAL_EXPAND Interval_Expand;
APIGET FPINTERVAL_OFFSET Interval_Offset;
APIGET FPINTERVAL_CONTAINS Interval_Contains;
APIGET FPINTERVAL_CLAMP Interval_Clamp;
APIGET FPINTERVAL_UNION Interval_Union;
APIGET FPAABB_GENERATEFROMOBJECTS AABB_GenerateFromObjects;
APIGET FPAABB_GENERATEFROMPOINTS AABB_GenerateFromPoints;
APIGET FPAABB_PADTOMINIMUMS AABB_PadToMinimums;
APIGET FPAABB_LONGESTAXIS AABB_LongestAxis;
APIGET FPCONST&AABB_AXISINTERVAL const&AABB_AxisInterval;
APIGET FPAABB_OFFSET AABB_Offset;
APIGET FPAABB_COMBINE AABB_Combine;
APIGET FPAABB_EXPAND AABB_Expand;
APIGET FPAABB_COLLIDESWITH AABB_CollidesWith;
APIGET FPCOLLISIONTEMPLATEPLANE_CONSTRUCT CollisionTemplatePlane_Construct;
APIGET FPCOLLISIONTEMPLATEPLANE_MOVEINTOCOORDINATESPACE CollisionTemplatePlane_MoveIntoCoordinateSpace;
APIGET FPEXPLODEDOBJECTLIST_DELETEALL ExplodedObjectList_DeleteAll;
APIGET FPPUMPKIN_REGISTEROBJECT Pumpkin_RegisterObject;
APIGET FPPUMPKIN_GETOBJECT Pumpkin_GetObject;
APIGET FPPUMPKIN_DELETEOBJECT Pumpkin_DeleteObject;
APIGET FPOBJECT_GETNAME Object_GetName;
APIGET FPOBJECT_SETMODEL Object_SetModel;
APIGET FPOBJECT_GETMODEL Object_GetModel;
APIGET FPOBJECT_ADDSCRIPT Object_AddScript;
APIGET FPOBJECT_GETSCRIPT Object_GetScript;
APIGET FPOBJECT_REMOVESCRIPT Object_RemoveScript;
APIGET FPOBJECT_GETALLSCRIPTS Object_GetAllScripts;
APIGET FPOBJECT_ADDDELETECALLBACK Object_AddDeleteCallback;
APIGET FPOBJECT_REMOVEDELETECALLBACK Object_RemoveDeleteCallback;
APIGET FPOBJECT_DUPLICATE Object_Duplicate;
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
APIGET FPPUMPKIN_REGISTERFORMAT Pumpkin_RegisterFormat;
APIGET FPPUMPKIN_GETFORMAT Pumpkin_GetFormat;
APIGET FPCONST*CONSTFORMAT_GETATTRIBUTEOFNAME const*constFormat_GetAttributeOfName;
APIGET FPPUMPKIN_REGISTERMESH Pumpkin_RegisterMesh;
APIGET FPPUMPKIN_GETMESH Pumpkin_GetMesh;
APIGET FPPUMPKIN_APPLYSTATICBUFFER Pumpkin_ApplyStaticBuffer;
APIGET FPMESH_GETINFO Mesh_GetInfo;
APIGET FPMESH_RELOAD Mesh_Reload;
APIGET FPMESH_GETNAME Mesh_GetName;
APIGET FPMESH_DUPLICATEASDYNAMIC Mesh_DuplicateAsDynamic;
APIGET FPPUMPKIN_REGISTERMODEL Pumpkin_RegisterModel;
APIGET FPPUMPKIN_GETMODEL Pumpkin_GetModel;
APIGET FPMODEL_SETSHADER Model_SetShader;
APIGET FPMODEL_SETMESH Model_SetMesh;
APIGET FPMODEL_GETSHADER Model_GetShader;
APIGET FPMODEL_GETMESH Model_GetMesh;
APIGET FPMODEL_GETNAME Model_GetName;
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