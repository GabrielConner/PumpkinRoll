#ifdef PUMPKIN_ROLL_PROD
#include "private/functions.h"
#else
#include "pumpkinFunctions.h"
#endif
#include "pumpkin/types.h"
#include "pumpkin/constants.h"

using namespace ::pPack;


namespace pumpkin {

// CollisionTemplatePlane
// --------------------------------------------------
// --------------------------------------------------

AABB CollisionTemplatePlane::GenerateAABB() const {
  return AABB_Combine(AABB_GenerateFromPoints(origin + u, origin + v), AABB_GenerateFromPoints(origin, origin + u + v));
}

// --------------------------------------------------
// --------------------------------------------------
// CollisionTemplatePlane





// CollisionTriangle
// --------------------------------------------------
// --------------------------------------------------

bool CollisionTriangle::Collide(::pumpkin::Ray const& ray, ::pumpkin::Interval interval, ::pumpkin::RayHitInfo& hit) const {
  double denom = DVector3::Dot(n, ray.direction);
  if (std::fabs(denom) < _PR_DELTA) return false;

  double t = (D - DVector3::Dot(n, ray.origin)) / denom;
  if (!::pumpkin::Interval_Contains(interval, t)) return false;

  DVector3 point = Ray_At(ray, t);
  DVector2 uv;
  CollisionTemplatePlane_MoveIntoCoordinateSpace(*this, point, uv);


  if (uv.x >= 0 && uv.y >= 0 && (uv.x + uv.y) <= 1) {
    hit.time = t;
    hit.position = point;
    hit.object = object;
    RayHitInfo_SetFaceNormal(hit, ray, n);
    return true;
  }
  return false;
}

// --------------------------------------------------
// --------------------------------------------------
// CollisionTriangle





// CollisionQuad
// --------------------------------------------------
// --------------------------------------------------

bool CollisionQuad::Collide(Ray const& ray, Interval interval, RayHitInfo& hit) const {
  double denom = DVector3::Dot(n, ray.direction);
  if (std::fabs(denom) < _PR_DELTA) return false;

  double t = (D - DVector3::Dot(n, ray.origin)) / denom;
  if (!Interval_Contains(interval, t)) return false;

  DVector3 point = Ray_At(ray, t);
  DVector2 uv;
  CollisionTemplatePlane_MoveIntoCoordinateSpace(*this, point, uv);

  if (uv.x >= 0 && uv.y >= 0 && uv.x <= 1 && uv.y <= 1) {
    hit.time = t;
    hit.position = point;
    hit.object = object;
    RayHitInfo_SetFaceNormal(hit, ray, n);
    return true;
  }
  return false;
}

// --------------------------------------------------
// --------------------------------------------------
// CollisionQuad





// CollisionSphere
// --------------------------------------------------
// --------------------------------------------------

AABB CollisionSphere::GenerateAABB() const {
  return AABB_GenerateFromPoints(center - radius, center + radius);
}



bool CollisionSphere::Collide(::pumpkin::Ray const& ray, ::pumpkin::Interval interval, ::pumpkin::RayHitInfo& hit) const {
  DVector3 diff = center - ray.origin;

  double a = DVector3::Dot(ray.direction, ray.direction);
  double h = DVector3::Dot(ray.direction, diff);
  double b = -2.0 * h;
  double c = DVector3::Dot(diff, diff) - (radius * radius);

  double sq = h * h - (a * c);
  if (sq < 0) return false;
  sq = std::sqrt(sq);

  double ans = (h - sq) / a;
  if (!Interval_Contains(interval, ans)) {
    ans = (h + sq) / a;
    if (!Interval_Contains(interval, ans)) return false;
  }

  hit.time = ans;
  hit.position = Ray_At(ray, hit.time);
  hit.object = object;
  DVector3 outwardNormal = (hit.position - center) / radius;
  RayHitInfo_SetFaceNormal(hit, ray, outwardNormal);

  return true;
}

// --------------------------------------------------
// --------------------------------------------------
// CollisionSphere



// CollisionList
// --------------------------------------------------
// --------------------------------------------------

void CollisionList::Add(CollisionObject* obj) {
  list.push_back(obj);
  bbox = AABB_Combine(bbox, obj->GenerateAABB());
}



void CollisionList::RegenerateAABB() {
  bbox = AABB();
  for (CollisionObject* obj : list) {
    bbox = AABB_Combine(bbox, obj->GenerateAABB());
  }
}



bool CollisionList::Collide(Ray const& ray, Interval interval, RayHitInfo& hit) const {
  bool hitAny = false;
  RayHitInfo inHit;

  for (CollisionObject* obj : list) {
    if (obj->Collide(ray, interval, inHit)) {
      hitAny = true;
      interval.max = inHit.time;
    }
  }

  if (hitAny)
    hit = inHit;
  return hitAny;
}



AABB CollisionList::GenerateAABB() const {
  return bbox;
}



void CollisionList::DeleteInternal() {
  for (CollisionObject* obj : list) {
    obj->DeleteInternal();
    delete(obj);
  }
  list.clear();
}

// --------------------------------------------------
// --------------------------------------------------
// CollisionList


}; // namespace pumpkin