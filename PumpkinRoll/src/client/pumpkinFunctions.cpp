/*
*
* Function Declarations Source
* Built 2026-06-26 10:03 PM
*
*/

#include "pumpkinFunctions.h"

namespace pumpkin {

FPPUMPKIN_INIT Pumpkin_Init = NULL;
FPPUMPKIN_UPDATE Pumpkin_Update = NULL;
FPPUMPKIN_END Pumpkin_End = NULL;
FPPUMPKIN_GETRUNTIME Pumpkin_GetRuntime = NULL;
FPPUMPKIN_EXECUTABLELOCATION Pumpkin_ExecutableLocation = NULL;
FPPUMPKIN_PRINTERROR Pumpkin_PrintError = NULL;
FPPUMPKIN_DELTATIME Pumpkin_DeltaTime = NULL;
FPPUMPKIN_TOTALTIME Pumpkin_TotalTime = NULL;
FPPUMPKIN_REGISTEROBJECT Pumpkin_RegisterObject = NULL;
FPPUMPKIN_GETOBJECT Pumpkin_GetObject = NULL;
FPPUMPKIN_DELETEOBJECT Pumpkin_DeleteObject = NULL;
FPOBJECT_GETNAME Object_GetName = NULL;
FPOBJECT_SETMODEL Object_SetModel = NULL;
FPOBJECT_ADDSCRIPT Object_AddScript = NULL;
FPOBJECT_GETSCRIPT Object_GetScript = NULL;
FPOBJECT_REMOVESCRIPT Object_RemoveScript = NULL;
FPOBJECT_ADDDELETECALLBACK Object_AddDeleteCallback = NULL;
FPOBJECT_REMOVEDELETECALLBACK Object_RemoveDeleteCallback = NULL;
FPTRANSFORM_GENERATEMODEL Transform_GenerateModel = NULL;
FPPUMPKIN_REGISTERCAMERA Pumpkin_RegisterCamera = NULL;
FPPUMPKIN_GETCAMERA Pumpkin_GetCamera = NULL;
FPPUMPKIN_SETPRIMARYCAMERA Pumpkin_SetPrimaryCamera = NULL;
FPPUMPKIN_GETPRIMARYCAMERA Pumpkin_GetPrimaryCamera = NULL;
FPCAMERA_GENERATEVIEW Camera_GenerateView = NULL;
FPCAMERA_GENERATEPROJECTION Camera_GenerateProjection = NULL;
FPCAMERA_FORWARD Camera_Forward = NULL;
FPCAMERA_RIGHT Camera_Right = NULL;
FPCAMERA_GETANGLEBASED Camera_GetAngleBased = NULL;
FPCAMERA_ANGLEBASED Camera_AngleBased = NULL;
FPCAMERA_LOOKATTARGET Camera_LookAtTarget = NULL;
FPCAMERA_GETLOOKATTARGET Camera_GetLookAtTarget = NULL;
FPPUMPKIN_REGISTERMESH Pumpkin_RegisterMesh = NULL;
FPPUMPKIN_GETMESH Pumpkin_GetMesh = NULL;
FPPUMPKIN_REGISTERFORMAT Pumpkin_RegisterFormat = NULL;
FPPUMPKIN_GETFORMAT Pumpkin_GetFormat = NULL;
FPPUMPKIN_APPLYSTATICBUFFER Pumpkin_ApplyStaticBuffer = NULL;
FPMESH_GETMESHVERTICES Mesh_GetMeshVertices = NULL;
FPMESH_RELOAD Mesh_Reload = NULL;
FPPUMPKIN_REGISTERMODEL Pumpkin_RegisterModel = NULL;
FPPUMPKIN_GETMODEL Pumpkin_GetModel = NULL;
FPMODEL_SETSHADER Model_SetShader = NULL;
FPMODEL_SETMESH Model_SetMesh = NULL;
FPMODEL_GETPROPERTIES Model_GetProperties = NULL;
FPPUMPKIN_REGISTERSHADER Pumpkin_RegisterShader = NULL;
FPPUMPKIN_GETSHADER Pumpkin_GetShader = NULL;
FPSHADER_GETPROPERTIES Shader_GetProperties = NULL;
FPPROPERTYHOLDER_ADDPROPERTY PropertyHolder_AddProperty = NULL;
FPPROPERTYHOLDER_SETPROPERTY PropertyHolder_SetProperty = NULL;
FPPROPERTYHOLDER_SETORADDPROPERTY PropertyHolder_SetOrAddProperty = NULL;
FPPROPERTYHOLDER_DELETEPROPERTY PropertyHolder_DeleteProperty = NULL;
FPPROPERTYHOLDER_GETPROPERTY PropertyHolder_GetProperty = NULL;
FPPUMPKIN_REGISTERSCRIPTRAW Pumpkin_RegisterScriptRaw = NULL;
FPPUMPKIN_CREATESCRIPT Pumpkin_CreateScript = NULL;
FPPUMPKIN_GETSCRIPTNAME Pumpkin_GetScriptName = NULL;
FPPUMPKIN_LOADFILE Pumpkin_LoadFile = NULL;
FPPUMPKIN_FORGETFILE Pumpkin_ForgetFile = NULL;
FPPUMPKIN_READFILE Pumpkin_ReadFile = NULL;
FPPUMPKIN_TORELATIVEPATH Pumpkin_ToRelativePath = NULL;
FPPUMPKIN_OPENFILEFUNC Pumpkin_OpenFileFunc = NULL;
FPPUMPKIN_CLOSEFILEFUNC Pumpkin_CloseFileFunc = NULL;


bool LoadFunctions(PROCADDRESSFUNC proc) {
  if (!proc) return false;

  Pumpkin_Init = (FPPUMPKIN_INIT)proc("Pumpkin_Init"); if (!Pumpkin_Init) return false;
  Pumpkin_Update = (FPPUMPKIN_UPDATE)proc("Pumpkin_Update"); if (!Pumpkin_Update) return false;
  Pumpkin_End = (FPPUMPKIN_END)proc("Pumpkin_End"); if (!Pumpkin_End) return false;
  Pumpkin_GetRuntime = (FPPUMPKIN_GETRUNTIME)proc("Pumpkin_GetRuntime"); if (!Pumpkin_GetRuntime) return false;
  Pumpkin_ExecutableLocation = (FPPUMPKIN_EXECUTABLELOCATION)proc("Pumpkin_ExecutableLocation"); if (!Pumpkin_ExecutableLocation) return false;
  Pumpkin_PrintError = (FPPUMPKIN_PRINTERROR)proc("Pumpkin_PrintError"); if (!Pumpkin_PrintError) return false;
  Pumpkin_DeltaTime = (FPPUMPKIN_DELTATIME)proc("Pumpkin_DeltaTime"); if (!Pumpkin_DeltaTime) return false;
  Pumpkin_TotalTime = (FPPUMPKIN_TOTALTIME)proc("Pumpkin_TotalTime"); if (!Pumpkin_TotalTime) return false;
  Pumpkin_RegisterObject = (FPPUMPKIN_REGISTEROBJECT)proc("Pumpkin_RegisterObject"); if (!Pumpkin_RegisterObject) return false;
  Pumpkin_GetObject = (FPPUMPKIN_GETOBJECT)proc("Pumpkin_GetObject"); if (!Pumpkin_GetObject) return false;
  Pumpkin_DeleteObject = (FPPUMPKIN_DELETEOBJECT)proc("Pumpkin_DeleteObject"); if (!Pumpkin_DeleteObject) return false;
  Object_GetName = (FPOBJECT_GETNAME)proc("Object_GetName"); if (!Object_GetName) return false;
  Object_SetModel = (FPOBJECT_SETMODEL)proc("Object_SetModel"); if (!Object_SetModel) return false;
  Object_AddScript = (FPOBJECT_ADDSCRIPT)proc("Object_AddScript"); if (!Object_AddScript) return false;
  Object_GetScript = (FPOBJECT_GETSCRIPT)proc("Object_GetScript"); if (!Object_GetScript) return false;
  Object_RemoveScript = (FPOBJECT_REMOVESCRIPT)proc("Object_RemoveScript"); if (!Object_RemoveScript) return false;
  Object_AddDeleteCallback = (FPOBJECT_ADDDELETECALLBACK)proc("Object_AddDeleteCallback"); if (!Object_AddDeleteCallback) return false;
  Object_RemoveDeleteCallback = (FPOBJECT_REMOVEDELETECALLBACK)proc("Object_RemoveDeleteCallback"); if (!Object_RemoveDeleteCallback) return false;
  Transform_GenerateModel = (FPTRANSFORM_GENERATEMODEL)proc("Transform_GenerateModel"); if (!Transform_GenerateModel) return false;
  Pumpkin_RegisterCamera = (FPPUMPKIN_REGISTERCAMERA)proc("Pumpkin_RegisterCamera"); if (!Pumpkin_RegisterCamera) return false;
  Pumpkin_GetCamera = (FPPUMPKIN_GETCAMERA)proc("Pumpkin_GetCamera"); if (!Pumpkin_GetCamera) return false;
  Pumpkin_SetPrimaryCamera = (FPPUMPKIN_SETPRIMARYCAMERA)proc("Pumpkin_SetPrimaryCamera"); if (!Pumpkin_SetPrimaryCamera) return false;
  Pumpkin_GetPrimaryCamera = (FPPUMPKIN_GETPRIMARYCAMERA)proc("Pumpkin_GetPrimaryCamera"); if (!Pumpkin_GetPrimaryCamera) return false;
  Camera_GenerateView = (FPCAMERA_GENERATEVIEW)proc("Camera_GenerateView"); if (!Camera_GenerateView) return false;
  Camera_GenerateProjection = (FPCAMERA_GENERATEPROJECTION)proc("Camera_GenerateProjection"); if (!Camera_GenerateProjection) return false;
  Camera_Forward = (FPCAMERA_FORWARD)proc("Camera_Forward"); if (!Camera_Forward) return false;
  Camera_Right = (FPCAMERA_RIGHT)proc("Camera_Right"); if (!Camera_Right) return false;
  Camera_GetAngleBased = (FPCAMERA_GETANGLEBASED)proc("Camera_GetAngleBased"); if (!Camera_GetAngleBased) return false;
  Camera_AngleBased = (FPCAMERA_ANGLEBASED)proc("Camera_AngleBased"); if (!Camera_AngleBased) return false;
  Camera_LookAtTarget = (FPCAMERA_LOOKATTARGET)proc("Camera_LookAtTarget"); if (!Camera_LookAtTarget) return false;
  Camera_GetLookAtTarget = (FPCAMERA_GETLOOKATTARGET)proc("Camera_GetLookAtTarget"); if (!Camera_GetLookAtTarget) return false;
  Pumpkin_RegisterMesh = (FPPUMPKIN_REGISTERMESH)proc("Pumpkin_RegisterMesh"); if (!Pumpkin_RegisterMesh) return false;
  Pumpkin_GetMesh = (FPPUMPKIN_GETMESH)proc("Pumpkin_GetMesh"); if (!Pumpkin_GetMesh) return false;
  Pumpkin_RegisterFormat = (FPPUMPKIN_REGISTERFORMAT)proc("Pumpkin_RegisterFormat"); if (!Pumpkin_RegisterFormat) return false;
  Pumpkin_GetFormat = (FPPUMPKIN_GETFORMAT)proc("Pumpkin_GetFormat"); if (!Pumpkin_GetFormat) return false;
  Pumpkin_ApplyStaticBuffer = (FPPUMPKIN_APPLYSTATICBUFFER)proc("Pumpkin_ApplyStaticBuffer"); if (!Pumpkin_ApplyStaticBuffer) return false;
  Mesh_GetMeshVertices = (FPMESH_GETMESHVERTICES)proc("Mesh_GetMeshVertices"); if (!Mesh_GetMeshVertices) return false;
  Mesh_Reload = (FPMESH_RELOAD)proc("Mesh_Reload"); if (!Mesh_Reload) return false;
  Pumpkin_RegisterModel = (FPPUMPKIN_REGISTERMODEL)proc("Pumpkin_RegisterModel"); if (!Pumpkin_RegisterModel) return false;
  Pumpkin_GetModel = (FPPUMPKIN_GETMODEL)proc("Pumpkin_GetModel"); if (!Pumpkin_GetModel) return false;
  Model_SetShader = (FPMODEL_SETSHADER)proc("Model_SetShader"); if (!Model_SetShader) return false;
  Model_SetMesh = (FPMODEL_SETMESH)proc("Model_SetMesh"); if (!Model_SetMesh) return false;
  Model_GetProperties = (FPMODEL_GETPROPERTIES)proc("Model_GetProperties"); if (!Model_GetProperties) return false;
  Pumpkin_RegisterShader = (FPPUMPKIN_REGISTERSHADER)proc("Pumpkin_RegisterShader"); if (!Pumpkin_RegisterShader) return false;
  Pumpkin_GetShader = (FPPUMPKIN_GETSHADER)proc("Pumpkin_GetShader"); if (!Pumpkin_GetShader) return false;
  Shader_GetProperties = (FPSHADER_GETPROPERTIES)proc("Shader_GetProperties"); if (!Shader_GetProperties) return false;
  PropertyHolder_AddProperty = (FPPROPERTYHOLDER_ADDPROPERTY)proc("PropertyHolder_AddProperty"); if (!PropertyHolder_AddProperty) return false;
  PropertyHolder_SetProperty = (FPPROPERTYHOLDER_SETPROPERTY)proc("PropertyHolder_SetProperty"); if (!PropertyHolder_SetProperty) return false;
  PropertyHolder_SetOrAddProperty = (FPPROPERTYHOLDER_SETORADDPROPERTY)proc("PropertyHolder_SetOrAddProperty"); if (!PropertyHolder_SetOrAddProperty) return false;
  PropertyHolder_DeleteProperty = (FPPROPERTYHOLDER_DELETEPROPERTY)proc("PropertyHolder_DeleteProperty"); if (!PropertyHolder_DeleteProperty) return false;
  PropertyHolder_GetProperty = (FPPROPERTYHOLDER_GETPROPERTY)proc("PropertyHolder_GetProperty"); if (!PropertyHolder_GetProperty) return false;
  Pumpkin_RegisterScriptRaw = (FPPUMPKIN_REGISTERSCRIPTRAW)proc("Pumpkin_RegisterScriptRaw"); if (!Pumpkin_RegisterScriptRaw) return false;
  Pumpkin_CreateScript = (FPPUMPKIN_CREATESCRIPT)proc("Pumpkin_CreateScript"); if (!Pumpkin_CreateScript) return false;
  Pumpkin_GetScriptName = (FPPUMPKIN_GETSCRIPTNAME)proc("Pumpkin_GetScriptName"); if (!Pumpkin_GetScriptName) return false;
  Pumpkin_LoadFile = (FPPUMPKIN_LOADFILE)proc("Pumpkin_LoadFile"); if (!Pumpkin_LoadFile) return false;
  Pumpkin_ForgetFile = (FPPUMPKIN_FORGETFILE)proc("Pumpkin_ForgetFile"); if (!Pumpkin_ForgetFile) return false;
  Pumpkin_ReadFile = (FPPUMPKIN_READFILE)proc("Pumpkin_ReadFile"); if (!Pumpkin_ReadFile) return false;
  Pumpkin_ToRelativePath = (FPPUMPKIN_TORELATIVEPATH)proc("Pumpkin_ToRelativePath"); if (!Pumpkin_ToRelativePath) return false;
  Pumpkin_OpenFileFunc = (FPPUMPKIN_OPENFILEFUNC)proc("Pumpkin_OpenFileFunc"); if (!Pumpkin_OpenFileFunc) return false;
  Pumpkin_CloseFileFunc = (FPPUMPKIN_CLOSEFILEFUNC)proc("Pumpkin_CloseFileFunc"); if (!Pumpkin_CloseFileFunc) return false;

  return true;
}

}