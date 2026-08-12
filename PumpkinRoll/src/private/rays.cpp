#include "pumpkin/types.h"
#include "private/functions.h"
#include "pumpkin/pumpkinRoll.h"
#include "private/pumpkinRoll.h"

#include "pPack/vector.h"

using namespace ::pPack;

namespace pumpkin {

ExplodedObjectList Pumpkin_ExplodeAllObjects() {
  return ExplodedObjectList();
}



bool Pumpkin_CastRay(Ray const& ray, RayHitInfo& hit) {
  return false;
}



bool Pumpkin_CastRayInto(Ray const& ray, RayHitInfo& hit, ExplodedObjectList const& list) {
  Interval interval = {_PR_DELTA, _PR_INFINITY};
  bool hitAny = false;

  for (ExplodedObject const& obj : list) {
    for (CollisionObject const* collider : obj.mesh) {
      if (!collider->Collide(ray, interval, hit)) continue;

      interval.max = hit.time;
      hit.object = obj.object;
      hitAny = true;
    }
  }

  return hitAny;
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



Interval Interval_Expand(Interval const& interval, double delta) {
  auto padding = delta / 2.0f;
  return Interval(interval.min - padding, interval.max + padding);
}



bool Interval_Contains(Interval const& interval, double value) {
  return interval.min <= value && interval.max >= value;
}



double Interval_Clamp(Interval const& interval, double value) {
  if (interval.min < value) return interval.min;
  if (interval.max > value) return interval.max;
  return value;
}

// --------------------------------------------------
// --------------------------------------------------
// Interval





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





// ExplodedObject
// --------------------------------------------------
// --------------------------------------------------

void ExplodedObject_Delete(ExplodedObject& object) {
  for (CollisionObject* collider : object.mesh) {
    delete(collider);
  }
  object.mesh.clear();
}

// --------------------------------------------------
// --------------------------------------------------
// ExplodedObject





// ExplodedObject
// --------------------------------------------------
// --------------------------------------------------

void ExplodedObjectList_DeleteAll(ExplodedObjectList& list) {
  for (ExplodedObject& object : list) {
    ExplodedObject_Delete(object);
  }
  list.list.clear();
}

// --------------------------------------------------
// --------------------------------------------------
// ExplodedObject

}; // namespace pumpkin