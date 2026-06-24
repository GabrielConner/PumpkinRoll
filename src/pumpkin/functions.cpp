/*
*
* Function Declarations Source
* Built 2026-06-23 11:16 PM
*
*/

#include "functions.h"

namespace pumpkin {

FPINIT Init = NULL;
FPUPDATE Update = NULL;
FPEND End = NULL;
FPGETRUNTIME GetRuntime = NULL;
FPEXECUTABLELOCATION ExecutableLocation = NULL;
FPPRINTERROR PrintError = NULL;
FPDELTATIME DeltaTime = NULL;
FPREGISTEROBJECT RegisterObject = NULL;
FPGETOBJECT GetObject = NULL;
FPDELETEOBJECT DeleteObject = NULL;
FPOBJECT_GETNAME Object_GetName = NULL;
FPOBJECT_SETMODEL Object_SetModel = NULL;
FPOBJECT_ADDSCRIPT Object_AddScript = NULL;
FPOBJECT_GETSCRIPT Object_GetScript = NULL;
FPOBJECT_REMOVESCRIPT Object_RemoveScript = NULL;
FPOBJECT_ADDDELETECALLBACK Object_AddDeleteCallback = NULL;
FPOBJECT_REMOVEDELETECALLBACK Object_RemoveDeleteCallback = NULL;
FPTRANSFORM_GENERATEMODEL Transform_GenerateModel = NULL;
FPREGISTERCAMERA RegisterCamera = NULL;
FPGETCAMERA GetCamera = NULL;
FPSETPRIMARYCAMERA SetPrimaryCamera = NULL;
FPGETPRIMARYCAMERA GetPrimaryCamera = NULL;
FPCAMERA_GENERATEVIEW Camera_GenerateView = NULL;
FPCAMERA_GENERATEPROJECTION Camera_GenerateProjection = NULL;
FPCAMERA_FORWARD Camera_Forward = NULL;
FPCAMERA_RIGHT Camera_Right = NULL;
FPCAMERA_GETANGLEBASED Camera_GetAngleBased = NULL;
FPCAMERA_ANGLEBASED Camera_AngleBased = NULL;
FPCAMERA_LOOKATTARGET Camera_LookAtTarget = NULL;
FPCAMERA_GETLOOKATTARGET Camera_GetLookAtTarget = NULL;
FPREGISTERMESH RegisterMesh = NULL;
FPGETMESH GetMesh = NULL;
FPREGISTERFORMAT RegisterFormat = NULL;
FPGETFORMAT GetFormat = NULL;
FPAPPLYSTATICBUFFER ApplyStaticBuffer = NULL;
FPREGISTERMODEL RegisterModel = NULL;
FPGETMODEL GetModel = NULL;
FPMODEL_SETSHADER Model_SetShader = NULL;
FPMODEL_SETMESH Model_SetMesh = NULL;
FPMODEL_GETPROPERTIES Model_GetProperties = NULL;
FPREGISTERSHADER RegisterShader = NULL;
FPGETSHADER GetShader = NULL;
FPSHADER_GETPROPERTIES Shader_GetProperties = NULL;
FPPROPERTYHOLDER_ADDPROPERTY PropertyHolder_AddProperty = NULL;
FPPROPERTYHOLDER_SETPROPERTY PropertyHolder_SetProperty = NULL;
FPPROPERTYHOLDER_SETORADDPROPERTY PropertyHolder_SetOrAddProperty = NULL;
FPPROPERTYHOLDER_DELETEPROPERTY PropertyHolder_DeleteProperty = NULL;
FPPROPERTYHOLDER_GETPROPERTY PropertyHolder_GetProperty = NULL;
FPREGISTERSCRIPTRAW RegisterScriptRaw = NULL;
FPCREATESCRIPT CreateScript = NULL;
FPGETSCRIPTNAME GetScriptName = NULL;


bool LoadFunctions(PROCADDRESSFUNC proc) {
  if (!proc) return false;

  Init = (FPINIT)proc("Init"); if (!Init) return false;
  Update = (FPUPDATE)proc("Update"); if (!Update) return false;
  End = (FPEND)proc("End"); if (!End) return false;
  GetRuntime = (FPGETRUNTIME)proc("GetRuntime"); if (!GetRuntime) return false;
  ExecutableLocation = (FPEXECUTABLELOCATION)proc("ExecutableLocation"); if (!ExecutableLocation) return false;
  PrintError = (FPPRINTERROR)proc("PrintError"); if (!PrintError) return false;
  DeltaTime = (FPDELTATIME)proc("DeltaTime"); if (!DeltaTime) return false;
  RegisterObject = (FPREGISTEROBJECT)proc("RegisterObject"); if (!RegisterObject) return false;
  GetObject = (FPGETOBJECT)proc("GetObject"); if (!GetObject) return false;
  DeleteObject = (FPDELETEOBJECT)proc("DeleteObject"); if (!DeleteObject) return false;
  Object_GetName = (FPOBJECT_GETNAME)proc("Object_GetName"); if (!Object_GetName) return false;
  Object_SetModel = (FPOBJECT_SETMODEL)proc("Object_SetModel"); if (!Object_SetModel) return false;
  Object_AddScript = (FPOBJECT_ADDSCRIPT)proc("Object_AddScript"); if (!Object_AddScript) return false;
  Object_GetScript = (FPOBJECT_GETSCRIPT)proc("Object_GetScript"); if (!Object_GetScript) return false;
  Object_RemoveScript = (FPOBJECT_REMOVESCRIPT)proc("Object_RemoveScript"); if (!Object_RemoveScript) return false;
  Object_AddDeleteCallback = (FPOBJECT_ADDDELETECALLBACK)proc("Object_AddDeleteCallback"); if (!Object_AddDeleteCallback) return false;
  Object_RemoveDeleteCallback = (FPOBJECT_REMOVEDELETECALLBACK)proc("Object_RemoveDeleteCallback"); if (!Object_RemoveDeleteCallback) return false;
  Transform_GenerateModel = (FPTRANSFORM_GENERATEMODEL)proc("Transform_GenerateModel"); if (!Transform_GenerateModel) return false;
  RegisterCamera = (FPREGISTERCAMERA)proc("RegisterCamera"); if (!RegisterCamera) return false;
  GetCamera = (FPGETCAMERA)proc("GetCamera"); if (!GetCamera) return false;
  SetPrimaryCamera = (FPSETPRIMARYCAMERA)proc("SetPrimaryCamera"); if (!SetPrimaryCamera) return false;
  GetPrimaryCamera = (FPGETPRIMARYCAMERA)proc("GetPrimaryCamera"); if (!GetPrimaryCamera) return false;
  Camera_GenerateView = (FPCAMERA_GENERATEVIEW)proc("Camera_GenerateView"); if (!Camera_GenerateView) return false;
  Camera_GenerateProjection = (FPCAMERA_GENERATEPROJECTION)proc("Camera_GenerateProjection"); if (!Camera_GenerateProjection) return false;
  Camera_Forward = (FPCAMERA_FORWARD)proc("Camera_Forward"); if (!Camera_Forward) return false;
  Camera_Right = (FPCAMERA_RIGHT)proc("Camera_Right"); if (!Camera_Right) return false;
  Camera_GetAngleBased = (FPCAMERA_GETANGLEBASED)proc("Camera_GetAngleBased"); if (!Camera_GetAngleBased) return false;
  Camera_AngleBased = (FPCAMERA_ANGLEBASED)proc("Camera_AngleBased"); if (!Camera_AngleBased) return false;
  Camera_LookAtTarget = (FPCAMERA_LOOKATTARGET)proc("Camera_LookAtTarget"); if (!Camera_LookAtTarget) return false;
  Camera_GetLookAtTarget = (FPCAMERA_GETLOOKATTARGET)proc("Camera_GetLookAtTarget"); if (!Camera_GetLookAtTarget) return false;
  RegisterMesh = (FPREGISTERMESH)proc("RegisterMesh"); if (!RegisterMesh) return false;
  GetMesh = (FPGETMESH)proc("GetMesh"); if (!GetMesh) return false;
  RegisterFormat = (FPREGISTERFORMAT)proc("RegisterFormat"); if (!RegisterFormat) return false;
  GetFormat = (FPGETFORMAT)proc("GetFormat"); if (!GetFormat) return false;
  ApplyStaticBuffer = (FPAPPLYSTATICBUFFER)proc("ApplyStaticBuffer"); if (!ApplyStaticBuffer) return false;
  RegisterModel = (FPREGISTERMODEL)proc("RegisterModel"); if (!RegisterModel) return false;
  GetModel = (FPGETMODEL)proc("GetModel"); if (!GetModel) return false;
  Model_SetShader = (FPMODEL_SETSHADER)proc("Model_SetShader"); if (!Model_SetShader) return false;
  Model_SetMesh = (FPMODEL_SETMESH)proc("Model_SetMesh"); if (!Model_SetMesh) return false;
  Model_GetProperties = (FPMODEL_GETPROPERTIES)proc("Model_GetProperties"); if (!Model_GetProperties) return false;
  RegisterShader = (FPREGISTERSHADER)proc("RegisterShader"); if (!RegisterShader) return false;
  GetShader = (FPGETSHADER)proc("GetShader"); if (!GetShader) return false;
  Shader_GetProperties = (FPSHADER_GETPROPERTIES)proc("Shader_GetProperties"); if (!Shader_GetProperties) return false;
  PropertyHolder_AddProperty = (FPPROPERTYHOLDER_ADDPROPERTY)proc("PropertyHolder_AddProperty"); if (!PropertyHolder_AddProperty) return false;
  PropertyHolder_SetProperty = (FPPROPERTYHOLDER_SETPROPERTY)proc("PropertyHolder_SetProperty"); if (!PropertyHolder_SetProperty) return false;
  PropertyHolder_SetOrAddProperty = (FPPROPERTYHOLDER_SETORADDPROPERTY)proc("PropertyHolder_SetOrAddProperty"); if (!PropertyHolder_SetOrAddProperty) return false;
  PropertyHolder_DeleteProperty = (FPPROPERTYHOLDER_DELETEPROPERTY)proc("PropertyHolder_DeleteProperty"); if (!PropertyHolder_DeleteProperty) return false;
  PropertyHolder_GetProperty = (FPPROPERTYHOLDER_GETPROPERTY)proc("PropertyHolder_GetProperty"); if (!PropertyHolder_GetProperty) return false;
  RegisterScriptRaw = (FPREGISTERSCRIPTRAW)proc("RegisterScriptRaw"); if (!RegisterScriptRaw) return false;
  CreateScript = (FPCREATESCRIPT)proc("CreateScript"); if (!CreateScript) return false;
  GetScriptName = (FPGETSCRIPTNAME)proc("GetScriptName"); if (!GetScriptName) return false;

  return true;
}

}