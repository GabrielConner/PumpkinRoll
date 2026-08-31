#ifndef PUMPKIN_ROLL_SRC_PRIVATE_BVH_NODE_H
#define PUMPKIN_ROLL_SRC_PRIVATE_BVH_NODE_H

#include "pumpkin/types.h"
#include "private/functions.h"

namespace pumpkin_private {

struct BVHNodeReference {
  ::pumpkin::CollisionObject* obj = nullptr;
  size_t references = 0;

  ::pumpkin::CollisionObject* GetReference() {
    if (!obj) return nullptr;
    ++references;
    return obj;
  }

  size_t DeleteReference() {
    if (!references || !obj) return 0;
    if (!--references) {
      delete(obj);
      obj = nullptr;
    }
    return references;
  }

  BVHNodeReference() = default;
  BVHNodeReference(::pumpkin::CollisionObject* Obj) : obj(Obj) {}
};


struct BVHNodeSmartPointer {
  BVHNodeReference* pointer = nullptr;


  ::pumpkin::CollisionObject* operator->() const {
    return pointer->obj;
  }


  ::pumpkin::CollisionObject* GetReference() {
    if (!pointer) return nullptr;
    return pointer->GetReference();
  }

  void DeleteReference() {
    if (!pointer) return;
    if (!pointer->DeleteReference()) delete(pointer);
  }

  BVHNodeSmartPointer() = default;
  BVHNodeSmartPointer(::pumpkin::CollisionObject* obj) {
    pointer = new BVHNodeReference(obj);
    GetReference();
  }
};


struct BVHNode : ::pumpkin::CollisionObject {
  ::pumpkin::AABB bbox = ::pumpkin::AABB();
  BVHNodeSmartPointer left, right;

  bool Collide(::pumpkin::Ray const& ray, ::pumpkin::Interval interval, ::pumpkin::RayHitInfo& hit) const { // ensure left/right are not null
    if (!::pumpkin::AABB_CollidesWith(bbox, ray, interval)) return false;

    bool hitLeft = left->Collide(ray, interval, hit);
    bool hitRight = right->Collide(ray, ::pumpkin::Interval(interval.min, hitLeft ? hit.time : interval.max), hit);

    return hitLeft || hitRight;
  }


  ::pumpkin::AABB GenerateAABB() const override { return bbox; }


  void DeleteInternal() override {
    left->DeleteInternal();
    right->DeleteInternal();

    left.DeleteReference();
    right.DeleteReference();
  }



  BVHNode() = default;
  BVHNode(std::vector<::pumpkin::CollisionObject*>& list, size_t start, size_t end) {
    bbox = ::pumpkin::AABB_GenerateFromObjects(list, start, end);
    int axis = ::pumpkin::AABB_LongestAxis(bbox);

    auto comparator = (axis == 0) ? ObjectSortX 
    : (axis == 1) ? ObjectSortY
    : ObjectSortZ;

    size_t objSpan = (end - start);
    if (objSpan == 1) {
      left = list[start];
      right.pointer = left.pointer;
      right.GetReference();
    } else if (objSpan == 2) {
      left = list[start];
      right = list[start + 1];
    } else {
      std::sort(list.begin() + start, list.begin() + end, comparator);
      size_t mid = start + objSpan / 2;
      left = new BVHNode(list, start, mid); // left right might both contain mid if list has odd number of objects
      right = new BVHNode(list, mid, end);
    }
  }



  static bool ObjectSort(::pumpkin::CollisionObject const * const& a, ::pumpkin::CollisionObject const * const& b, int axis) {
    ::pumpkin::Interval const& aInterval = ::pumpkin::AABB_AxisInterval(a->GenerateAABB(), axis);
    ::pumpkin::Interval const& bInterval = ::pumpkin::AABB_AxisInterval(b->GenerateAABB(), axis);

    return aInterval.min < bInterval.min;
  }

  static bool ObjectSortX(CollisionObject const * const& a, CollisionObject const * const& b) {
    return ObjectSort(a, b, 0);
  }
  static bool ObjectSortY(CollisionObject const * const& a, CollisionObject const * const& b) {
    return ObjectSort(a, b, 1);
  }
  static bool ObjectSortZ(CollisionObject const * const& a, CollisionObject const * const& b) {
    return ObjectSort(a, b, 2);
  }
};

}; // namespace pumpkin

#endif