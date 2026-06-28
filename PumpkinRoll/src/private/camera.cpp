#include "private/camera.h"
#include "private/types.h"
#include "pumpkin/constants.h"

#include "pPack/vector.h"

#include <assert.h>

using namespace ::pPack;
using namespace ::pumpkin;
using namespace ::pumpkin_private;


namespace pumpkin_private {

void StartCamera(Camera* camera) {
  assert(camera != nullptr);

  pCamDefInt(camera, i);
  i->forward = Vector3(0, 0, -1);
  i->angleBased = true;
  i->lookAt = nullptr;
  i->right = Vector3(1, 0, 0);
}


void UpdateCamera(Camera* camera) {
  assert(camera != nullptr);

  pCamDefInt(camera, i);

  if (i->lookAt) {
    i->forward = (*i->lookAt - camera->transform.position).Normal();
  }
  if (i->angleBased && !i->lookAt) {
    Vector3 r = camera->transform.rotation * _DEG_TO_RAD;

    i->forward.x = -cos(r.x) * sin(r.y);
    i->forward.y = sin(r.x);
    i->forward.z = -cos(r.x) * cos(r.y);
  }

  i->right = Vector3::Cross(i->forward, _UP).Normal();
}

}; // namespace pumpkin_private