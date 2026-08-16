#include "pumpkin/types.h"
#include "private/functions.h"

namespace pumpkin {

struct BVHNode : ::pumpkin::CollisionObject {
  ::pumpkin::AABB bbox;
  ::pumpkin::CollisionObject* left, *right;

  bool Collide(Ray const& ray, Interval interval, RayHitInfo& hit) const {
    if (!AABB_CollidesWith(bbox, ray, interval)) return false;

    bool hitLeft = left->Collide(ray, interval, hit);
    bool hitRight = right->Collide(ray, Interval(interval.min, hitLeft ? hit.time : interval.max), hit);

    return hitLeft || hitRight;
  }


  AABB GenerateAABB() override { return bbox; }
};

}; // namespace pumpkin