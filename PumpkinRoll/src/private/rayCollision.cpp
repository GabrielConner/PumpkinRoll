#include "pumpkin/types.h"
#include "private/functions.h"
#include "pumpkin/pumpkinRoll.h"
#include "private/pumpkinRoll.h"
#include "private/bvhNode.h"
#include "private/mesh.h"
#include "private/model.h"

#include "pPack/vector.h"

#include "glm/ext.hpp"
#include "glm/common.hpp"

#include <assert.h>

using namespace ::pPack;
using namespace ::pumpkin_private;

namespace pumpkin {

// Assumes all meshes are made of triangles
ExplodedObjectList Pumpkin_ExplodeAllObjects() {
  ExplodedObjectList ret = ExplodedObjectList();

  Pumpkin* pumpkin = GetPumpkin();
  assert(pumpkin != nullptr);

  for (auto& objP : pumpkin->registeredObjects) {
    Object* obj = objP.second;
    pObjDefInt(obj, i);

    if (i->model == nullptr) continue;

    Mesh const* const mesh = i->model->mesh;
    if (mesh == nullptr) continue;

    if ((mesh->vertexCount % 3) != 0) continue; // Assumes triangles, still make sure has triangle number of vertices

    Format format = Mesh_GetInfo(mesh).format;
    FormatStartInfo const*const position = Format_GetAttributeOfName(format, AttributeName::POSITION);
    if (position == nullptr) continue;


    glm::mat4 model = glm::mat4(1);
    Transform_GenerateModel(obj->transform, *(MatrixWrapper*)(&model));


    for (size_t index = 0; index < mesh->vertexCount; index+=3) {
      CollisionTriangle* triangle = new CollisionTriangle();
      Vector3* vertexOrigin = (Vector3*)((char*)mesh->vertices + position->relativeOffset + (index * format.vertexStride));
      Vector3* u = (Vector3*)((char*)mesh->vertices + position->relativeOffset + ((index + 1) * format.vertexStride));
      Vector3* v = (Vector3*)((char*)mesh->vertices + position->relativeOffset + ((index + 2) * format.vertexStride));

      glm::vec4 moveVertexOrigin = model * glm::vec4(vertexOrigin->x, vertexOrigin->y, vertexOrigin->z, 1);
      glm::vec4 moveU = model * glm::vec4(u->x, u->y, u->z, 1);
      glm::vec4 moveV = model * glm::vec4(v->x, v->y, v->z, 1);

      *vertexOrigin = *(Vector3*)&moveVertexOrigin;
      *u = (*(Vector3*)&moveU) - *vertexOrigin;
      *v = (*(Vector3*)&moveV) - *vertexOrigin;


      triangle->origin = vertexOrigin->ConvertTo<double>();
      triangle->u = u->ConvertTo<double>();
      triangle->v = v->ConvertTo<double>();
      triangle->object = obj;

      CollisionTemplatePlane_Construct(*triangle);
      ret.list.push_back(triangle);
    }
  }

  return ret;
}



bool Pumpkin_CastRay(Ray const& ray, RayHitInfo& hit) {
  ExplodedObjectList list = Pumpkin_WrapBVHNodesAround(Pumpkin_ExplodeAllObjects());
  bool ret = Pumpkin_CastRayInto(ray, hit, list);
  ExplodedObjectList_DeleteAll(list);
  return ret;
}



bool Pumpkin_CastRayInto(Ray const& ray, RayHitInfo& hit, ExplodedObjectList const& list) {
  Interval interval = {_PR_DELTA, _PR_INFINITY};
  bool hitAny = false;

  for (CollisionObject const * const& collider : list) {
    if (!collider->Collide(ray, interval, hit)) continue;

    interval.max = hit.time;
    hitAny = true;
  }

  return hitAny;
}



ExplodedObjectList Pumpkin_WrapBVHNodesAround(ExplodedObjectList const& list) {
  if (list.list.size() == 0) return list;

  ExplodedObjectList ret;

  std::vector<CollisionObject*> tmpList = list.list;
  BVHNode* node = new BVHNode(tmpList, 0, list.list.size());
 
  ret.push_back(node);
  return ret;
}





// Ray
// --------------------------------------------------
// --------------------------------------------------

DVector3 Ray_At(Ray const& ray, double time) {
  return ray.origin + ray.direction * time;
}


// --------------------------------------------------
// --------------------------------------------------
// Ray





// Interval
// --------------------------------------------------
// --------------------------------------------------

double Interval_Size(Interval const& interval) {
  return interval.max - interval.min;
}



void Interval_Expand(Interval& interval, double delta) {
  auto padding = delta / 2.0f;

  interval.min -= padding;
  interval.max += padding;
}



void Interval_Offset(Interval& interval, double offset) {
  interval.min += offset;
  interval.max += offset;
}



bool Interval_Contains(Interval const& interval, double value) {
  return interval.min <= value && interval.max >= value;
}



double Interval_Clamp(Interval const& interval, double value) {
  if (interval.min < value) return interval.min;
  if (interval.max > value) return interval.max;
  return value;
}



Interval Interval_Union(Interval const& a, Interval const& b) {
  Interval ret = Interval();
  ret.min = a.min < b.min ? a.min : b.min;
  ret.max = a.max > b.max ? a.max : b.max;
  return ret;
}


// --------------------------------------------------
// --------------------------------------------------
// Interval





// AABB
// --------------------------------------------------
// --------------------------------------------------

AABB AABB_GenerateFromObjects(std::vector<CollisionObject*> const& objects, size_t start, size_t end) {
  size_t span = end - start;
  if (span == 0) return AABB();
  AABB ret = objects[start]->GenerateAABB();

  for (size_t i = start + 1; i < end; i++) {
    ret = AABB_Combine(ret, objects[i]->GenerateAABB());
  }

  AABB_PadToMinimums(ret);
  return ret;
}



AABB AABB_GenerateFromPoints(::pPack::DVector3 const& a, ::pPack::DVector3 const& b) {
  AABB ret;
  ret.x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
  ret.y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
  ret.z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);

  AABB_PadToMinimums(ret);
  return ret;
}



void AABB_PadToMinimums(AABB& aabb) {
  double delta = 0.0001;
  if (Interval_Size(aabb.x) < delta) Interval_Expand(aabb.x, delta);
  if (Interval_Size(aabb.y) < delta) Interval_Expand(aabb.y, delta);
  if (Interval_Size(aabb.z) < delta) Interval_Expand(aabb.z, delta);
}



int AABB_LongestAxis(AABB const& aabb) {
  if (Interval_Size(aabb.x) > Interval_Size(aabb.y))
    return Interval_Size(aabb.x) > Interval_Size(aabb.z) ? 0 : 2;
  else
    return Interval_Size(aabb.y) > Interval_Size(aabb.z) ? 1 : 2;
}



Interval const& AABB_AxisInterval(AABB const& aabb, int axis) {
  if (axis == 2) return aabb.z;
  if (axis == 1) return aabb.y;
  return aabb.x;
}



void AABB_Offset(AABB& aabb, ::pPack::DVector3 offset) {
  Interval_Offset(aabb.x, offset.x);
  Interval_Offset(aabb.y, offset.y);
  Interval_Offset(aabb.z, offset.z);
}



AABB AABB_Combine(AABB const& a, AABB const& b) {
  AABB ret;
  ret.x = Interval_Union(a.x, b.x);
  ret.y = Interval_Union(a.y, b.y);
  ret.z = Interval_Union(a.z, b.z);
  return ret;
}



void AABB_Expand(AABB& aabb, double delta) {
  Interval_Expand(aabb.x, delta);
  Interval_Expand(aabb.y, delta);
  Interval_Expand(aabb.z, delta);
}



bool AABB_CollidesWith(AABB const& aabb, Ray const& ray, Interval interval) {
  for (int i = 0; i < 3; i++) {
    Interval const& ax = AABB_AxisInterval(aabb, i);
    double const invD = 1.0 / ray.direction[i];

    double dM = (ax.min - ray.origin[i]);
    if (std::fabs(invD) == _PR_INFINITY && dM == 0) {
      continue;
    }
    double t0 = dM * invD;

    dM = (ax.max - ray.origin[i]);
    if (std::fabs(invD) == _PR_INFINITY && dM == 0) { 
      continue;
    }
    double t1 = dM * invD;

    if (t0 < t1) {
      if (t0 > interval.min) interval.min = t0;
      if (t1 < interval.max) interval.max = t1;
    } else {
      if (t1 > interval.min) interval.min = t1;
      if (t0 < interval.max) interval.max = t0;
    }

    if (interval.min >= interval.max) return false;
  }

  return true;
}

// --------------------------------------------------
// --------------------------------------------------
// AABB





// CollisionTemplatePlane
// --------------------------------------------------
// --------------------------------------------------

void CollisionTemplatePlane_Construct(CollisionTemplatePlane& plane) {
  DVector3 norm = DVector3::Cross(plane.u, plane.v);
  plane.n = norm.Normal();
  plane.w = norm / DVector3::Dot(norm, norm);
  plane.D = DVector3::Dot(plane.n, plane.origin);
}



void CollisionTemplatePlane_MoveIntoCoordinateSpace(CollisionTemplatePlane const& plane, DVector3 point, DVector2& out) {
  DVector3 p = point - plane.origin;

  out = DVector2(
    DVector3::Dot(plane.w, DVector3::Cross(p, plane.v)), // Alpha
    DVector3::Dot(plane.w, DVector3::Cross(plane.u, p))  // Beta
  );
}

// --------------------------------------------------
// --------------------------------------------------
// CollisionTemplatePlane





// ExplodedObjectList
// --------------------------------------------------
// --------------------------------------------------

void ExplodedObjectList_DeleteAll(ExplodedObjectList& list) {
  for (CollisionObject* object : list) {
    object->DeleteInternal();
    delete(object);
  }
  list.list.clear();
}

// --------------------------------------------------
// --------------------------------------------------
// ExplodedObjectList

}; // namespace pumpkin