#include "pPack/vector.h"

namespace pPack {


Vector2 stov2(std::string const& str) {
  Vector2 ret = 0;
  char a[1] = {0}, b[21] = {0};
  int n = sscanf_s(str.c_str(), "%20[^,],%20[^,]", a, b);
  if (n != 2) return ret;

  if (!sscanf_s(a, "%*[^0123456789-+ef]%f", &ret.x)) sscanf_s(a, "%f", &ret.x);
  if (!sscanf_s(b, "%*[^0123456789-+ef]%f", &ret.y)) sscanf_s(b, "%f", &ret.y);

  return ret;
}


Vector3 stov3(std::string const& str) {
  Vector3 ret = 0;
  char a[1] = {0}, b[21] = {0}, c[21] = {0};
  int n = sscanf_s(str.c_str(), "%20[^,],%20[^,],%20[^,]", a, b, c);
  if (n != 3) return ret;

  if (!sscanf_s(a, "%*[^0123456789-+ef]%f", &ret.x)) sscanf_s(a, "%f", &ret.x);
  sscanf_s(b, "%f", &ret.y);
  if (!sscanf_s(c, "%*[^0123456789-+ef]%f", &ret.z)) sscanf_s(c, "%f", &ret.z);

  return ret;
}


Vector4 stov4(std::string const& str) {
  Vector4 ret = 0;
  char a[21] = {0}, b[21] = {0}, c[21] = {0}, d[21] = {0};
  int n = sscanf_s(str.c_str(), "%20[^,],%20[^,],%20[^,],%20[^,]", a, b, c, d);
  if (n != 4) return ret;

  if (!sscanf_s(a, "%*[^0123456789-+ef]%f", &ret.x)) sscanf_s(a, "%f", &ret.x);
  sscanf_s(b, "%f", &ret.y);
  sscanf_s(c, "%f", &ret.z);
  if (!sscanf_s(d, "%*[^0123456789-+ef]%f", &ret.w)) sscanf_s(d, "%f", &ret.w);

  return ret;
}


}; // namespace pPack