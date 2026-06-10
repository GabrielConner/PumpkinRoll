#ifndef PPM_INCLUDE_PPACK_VECTOR_H
#define PPM_INCLUDE_PPACK_VECTOR_H

#include <iostream>
#include <cstdint>








namespace pPack {
//Vector2
//--------------------------------------------------
template <class T>
struct Vector2_s {
  union {
    T x;
    T r;
    T s;
    T u;
  };
  union {
    T y;
    T g;
    T t;
    T v;
  };


  friend Vector2_s<T> operator +(Vector2_s<T> a, const Vector2_s<T>& b) {
    a += b;
    return a;
  }
  friend Vector2_s<T> operator -(Vector2_s<T> a, const Vector2_s<T>& b) {
    a -= b;
    return a;
  }
  friend Vector2_s<T> operator *(Vector2_s<T> a, const Vector2_s<T>& b) {
    a *= b;
    return a;
  }
  friend Vector2_s<T> operator /(Vector2_s<T> a, const Vector2_s<T>& b) {
    a /= b;
    return a;
  }

  friend Vector2_s<T> operator +(Vector2_s<T> a, const T& t) {
    a.x += t;
    a.y += t;
    return a;
  }
  friend Vector2_s<T> operator -(Vector2_s<T> a, const T& t) {
    a.x -= t;
    a.y -= t;
    return a;
  }
  friend Vector2_s<T> operator *(Vector2_s<T> a, const T& t) {
    a.x *= t;
    a.y *= t;
    return a;
  }
  friend Vector2_s<T> operator /(Vector2_s<T> a, const T& t) {
    a.x /= t;
    a.y /= t;
    return a;
  }



  Vector2_s<T> operator -() const {
    return Vector2_s<T>(-x, -y);
  }

  Vector2_s<T>& operator +=(const Vector2_s<T>& t) {
    x += t.x;
    y += t.y;
    return *this;
  }
  Vector2_s<T>& operator -=(const Vector2_s<T>& t) {
    x -= t.x;
    y -= t.y;
    return *this;
  }
  Vector2_s<T>& operator *=(const Vector2_s<T>& t) {
    x *= t.x;
    y *= t.y;
    return *this;
  }
  Vector2_s<T>& operator /=(const Vector2_s<T>& t) {
    x /= t.x;
    y /= t.y;
    return *this;
  }

  Vector2_s<T>& operator +=(const T& t) {
    x += t;
    y += t;
    return *this;
  }
  Vector2_s<T>& operator -=(const T& t) {
    x -= t;
    y -= t;
    return *this;
  }
  Vector2_s<T>& operator *=(const T& t) {
    x *= t;
    y *= t;
    return *this;
  }
  Vector2_s<T>& operator /=(const T& t) {
    x /= t;
    y /= t;
    return *this;
  }



  friend bool operator ==(const Vector2_s<T>& a, const Vector2_s<T>& b) {
    return a.x == b.x && a.y == b.y;
  }
  friend bool operator !=(const Vector2_s<T>& a, const Vector2_s<T>& b) {
    return a.x != b.x || a.y != b.y;
  }
  friend bool operator ==(const Vector2_s<T>& a, const T& b) {
    return a.x == b && a.y == b;
  }
  friend bool operator !=(const Vector2_s<T>& a, const T& b) {
    return a.x != b || a.y != b;
  }



  friend std::ostream& operator <<(std::ostream& str, const Vector2_s<T>& v) {
    str << +v.x << ", " << +v.y;
    return str;
  }


  Vector2_s<T> Sqrt() {
    return {sqrt(x), sqrt(y)};
  }

  float Magnitude() {
    return sqrt(x * x + y * y);
  }

  float MagnitudeSquared() {
    return x * x + y * y;
  }

  Vector2_s<T> Normal() {
    return *this / Magnitude();
  }

  const Vector2_s<T>& Normalize() {
    *this = Normal();
    return *this;
  }

  bool NearZero() const {
    auto s = 1e-8;
    return (fabs(x) < s) && (fabs(y) < s);
  }

  template<class O>
  Vector2_s<O> ConvertTo() const {
    return Vector2_s<O>(O(x), O(y));
  }


  static Vector2_s<T> Reflect(const Vector2_s<T>& vec, const Vector2_s<T>& norm) {
    return vec - (2 * Dot(vec, norm) * norm);
  }

  static T Dot(const Vector2_s<T>& a, const Vector2_s<T>& b) {
    return a.x * b.x + a.y * b.y;
  }

  static Vector2_s<T> Proj(const Vector2_s<T>& a, const Vector2_s<T>& b) {
    return (Vector2_s<T>::Dot(a, b) / Vector2_s<T>::Dot(b, b)) * b;
  }

  constexpr Vector2_s() : x(T(0)), y(T(0)) {}
  constexpr Vector2_s(T t) : x(t), y(t) {}
  constexpr Vector2_s(T X, T Y) : x(X), y(Y) {}
};





//Vector3
//--------------------------------------------------
template <class T = float>
struct Vector3_s {
  union {
    T x;
    T r;
    T s;
  };
  union {
    T y;
    T g;
    T t;
  };
  union {
    T z;
    T b;
    T p;
  };

  friend Vector3_s<T> operator +(Vector3_s<T> a, const Vector3_s<T>& b) {
    a += b;
    return a;
  }
  friend Vector3_s<T> operator -(Vector3_s<T> a, const Vector3_s<T>& b) {
    a -= b;
    return a;
  }
  friend Vector3_s<T> operator *(Vector3_s<T> a, const Vector3_s<T>& b) {
    a *= b;
    return a;
  }
  friend Vector3_s<T> operator /(Vector3_s<T> a, const Vector3_s<T>& b) {
    a /= b;
    return a;
  }

  friend Vector3_s<T> operator +(Vector3_s<T> a, const T& t) {
    a.x += t;
    a.y += t;
    a.z += t;
    return a;
  }
  friend Vector3_s<T> operator -(Vector3_s<T> a, const T& t)  {
    a.x -= t;
    a.y -= t;
    a.z -= t;
    return a;
  }
  friend Vector3_s<T> operator *(Vector3_s<T> a, const T& t)  {
    a.x *= t;
    a.y *= t;
    a.z *= t;
    return a;
  }
  friend Vector3_s<T> operator /(Vector3_s<T> a, const T& t) {
    a.x /= t;
    a.y /= t;
    a.z /= t;
    return a;
  }



  Vector3_s<T>& operator +=(const Vector3_s<T>& t) {
    x += t.x;
    y += t.y;
    z += t.z;
    return *this;
  }
  Vector3_s<T>& operator -=(const Vector3_s<T>& t) {
    x -= t.x;
    y -= t.y;
    z -= t.z;
    return *this;
  }
  Vector3_s<T>& operator *=(const Vector3_s<T>& t) {
    x *= t.x;
    y *= t.y;
    z *= t.z;
    return *this;
  }
  Vector3_s<T>& operator /=(const Vector3_s<T>& t) {
    x /= t.x;
    y /= t.y;
    z /= t.z;
    return *this;
  }

  Vector3_s<T>& operator +=(const T& t) {
    x += t;
    y += t;
    z += t;
    return *this;
  }
  Vector3_s<T>& operator -=(const T& t) {
    x -= t;
    y -= t;
    z -= t;
    return *this;
  }
  Vector3_s<T>& operator *=(const T& t) {
    x *= t;
    y *= t;
    z *= t;
    return *this;
  }
  Vector3_s<T>& operator /=(const T& t) {
    x /= t;
    y /= t;
    z /= t;
    return *this;
  }



  Vector3_s<T> operator -() const {
    return Vector3_s<T>(-x, -y, -z);
  }

  friend bool operator ==(const Vector3_s<T>& a, const Vector3_s<T>& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
  }
  friend bool operator !=(const Vector3_s<T>& a, const Vector3_s<T>& b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
  }
  friend bool operator ==(const Vector3_s<T>& a, const T& b) {
    return a.x == b && a.y == b && a.z == b;
  }
  friend bool operator !=(const Vector3_s<T>& a, const T& b) {
    return a.x != b || a.y != b || a.z != b;
  }


  friend std::ostream& operator <<(std::ostream& str, const Vector3_s<T>& v) {
    str << +v.x << ", " << +v.y << ", " << +v.z;
    return str;
  }


  Vector3_s<T> Sqrt() {
    return {sqrt(x), sqrt(y), sqrt(z)};
  }

  float Magnitude() const {
    return sqrt(x * x + y * y + z * z);
  }

  float MagnitudeSquared() const {
    return x * x + y * y + z * z;
  }

  Vector3_s<T> Normal() const {
    return *this / Magnitude();
  }

  const Vector3_s<T>& Normalize() {
    *this = Normal();
    return *this;
  }

  bool NearZero() const {
    auto s = 1e-8;
    return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s);
  }


  template<class O>
  Vector3_s<O> ConvertTo() const {
    return Vector3_s<O>(O(x), O(y), O(z));
  }


  static Vector3_s<T> Reflect(const Vector3_s<T>& vec, const Vector3_s<T>& norm) {
    return vec - (2 * Dot(vec, norm) * norm);
  }

  static T Dot(const Vector3_s<T>& a, const Vector3_s<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  static Vector3_s<T> Cross(const Vector3_s<T>& a, const Vector3_s<T>& b) {
    return Vector3_s<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
  }

  static Vector3_s<T> Proj(const Vector3_s<T>& a, const Vector3_s<T>& b) {
    return (Vector3_s<T>::Dot(a, b) / Vector3_s<T>::Dot(b, b)) * b;
  }


  constexpr Vector3_s() : x(T(0)), y(T(0)), z(T(0)) {}
  constexpr Vector3_s(T t) : x(t), y(t), z(t) {}
  constexpr Vector3_s(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
  constexpr Vector3_s(const Vector2_s<T>& vec) : x(vec.x), y(vec.y), z(T(0)) {}
};





//Vector4
//--------------------------------------------------
template <class T = float>
struct Vector4_s {
  union {
    T x;
    T r;
    T s;
  };
  union {
    T y;
    T g;
    T t;
  };
  union {
    T z;
    T b;
    T p;
  };
  union {
    T w;
    T a;
    T q;
  };


  friend Vector4_s<T> operator +(Vector4_s<T> a, const Vector4_s<T>& b) {
    a += b;
    return a;
  }
  friend Vector4_s<T> operator -(Vector4_s<T> a, const Vector4_s<T>& b) {
    a -= b;
    return a;
  }
  friend Vector4_s<T> operator *(Vector4_s<T> a, const Vector4_s<T>& b) {
    a *= b;
    return a;
  }
  friend Vector4_s<T> operator /(Vector4_s<T> a, const Vector4_s<T>& b) {
    a /= b;
    return a;
  }

  friend Vector4_s<T> operator +(Vector4_s<T> a, const T& t) {
    a.x += t;
    a.y += t;
    a.z += t;
    a.w += t;
    return a;
  }
  friend Vector4_s<T> operator -(Vector4_s<T> a, const T& t) {
    a.x -= t;
    a.y -= t;
    a.z -= t;
    a.w -= t;
    return a;
  }
  friend Vector4_s<T> operator *(Vector4_s<T> a, const T& t) {
    a.x *= t;
    a.y *= t;
    a.z *= t;
    a.w *= t;
    return a;
  }
  friend Vector4_s<T> operator /(Vector4_s<T> a, const T& t)  {
    a.x /= t;
    a.y /= t;
    a.z /= t;
    a.w /= t;
    return a;
  }



  Vector4_s<T> operator -() const {
    return Vector4_s<T>(-x, -y, -z, -w);
  }

  Vector4_s<T>& operator +=(const Vector4_s<T>& t) {
    x += t.x;
    y += t.y;
    z += t.z;
    w += t.w;
    return *this;
  }
  Vector4_s<T>& operator -=(const Vector4_s<T>& t) {
    x -= t.x;
    y -= t.y;
    z -= t.z;
    w -= t.w;
    return *this;
  }
  Vector4_s<T>& operator *=(const Vector4_s<T>& t) {
    x *= t.x;
    y *= t.y;
    z *= t.z;
    w *= t.w;
    return *this;
  }
  Vector4_s<T>& operator /=(const Vector4_s<T>& t) {
    x /= t.x;
    y /= t.y;
    z /= t.z;
    w /= t.w;
    return *this;
  }

  Vector4_s<T>& operator +=(const T& t) {
    x += t;
    y += t;
    z += t;
    z += t;
    return *this;
  }
  Vector4_s<T>& operator -=(const T& t) {
    x -= t;
    y -= t;
    z -= t;
    z -= t;
    return *this;
  }
  Vector4_s<T>& operator *=(const T& t) {
    x *= t;
    y *= t;
    z *= t;
    z *= t;
    return *this;
  }
  Vector4_s<T>& operator /=(const T& t) {
    x /= t;
    y /= t;
    z /= t;
    z /= t;
    return *this;
  }


  friend bool operator ==(const Vector4_s<T>& a, const Vector4_s<T>& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
  }
  friend bool operator !=(const Vector4_s<T>& a, const Vector4_s<T>& b) {
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
  }
  friend bool operator ==(const Vector4_s<T>& a, const T& b) {
    return a.x == b && a.y == b && a.z == b && a.w == b;
  }
  friend bool operator !=(const Vector4_s<T>& a, const T& b) {
    return a.x != b || a.y != b || a.z != b || a.w != b;
  }



  friend std::ostream& operator <<(std::ostream& str, const Vector4_s<T>& v) {
    str << +v.x << ", " << +v.y << ", " << +v.z << ", " << +v.w;
    return str;
  }




  Vector4_s<T> Sqrt() {
    return {sqrt(x), sqrt(y), sqrt(z), sqrt(w)};
  }

  float Magnitude() const {
    return sqrt(x * x + y * y + z * z + w * w);
  }

  float MagnitudeSquared() const {
    return x * x + y * y + z * z + w * w;
  }

  Vector4_s<T> Normal() const {
    return *this / Magnitude();
  }

  const Vector4_s<T>& Normalize() {
    *this = Normal();
    return *this;
  }

  bool NearZero() const {
    auto s = 1e-8;
    return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s) && (fabs(w) < s);
  }

  template<class O>
  Vector4_s<O> ConvertTo() const {
    return Vector4_s<O>(O(x), O(y), O(z), O(w));
  }


  static Vector4_s<T> Reflect(const Vector4_s<T>& vec, const Vector4_s<T>& norm) {
    return vec - (2 * Dot(vec, norm) * norm);
  }

  static T Dot(const Vector4_s<T>& a, const Vector4_s<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  }

  static Vector4_s<T> Proj(const Vector4_s<T>& a, const Vector4_s<T>& b) {
    return (Vector4_s<T>::Dot(a, b) / Vector4_s<T>::Dot(b,b)) * b;
  }


  constexpr Vector4_s() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}
  constexpr Vector4_s(T t) : x(t), y(t), z(t), w(t) {}
  constexpr Vector4_s(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}
  constexpr Vector4_s(Vector2_s<T> vec) : x(vec.x), y(vec.y), z(T(0)), w(T(0)) {}
  constexpr Vector4_s(Vector3_s<T> vec) : x(vec.x), y(vec.y), z(vec.z), w(T(0)) {}
};


typedef struct Vector2_s<float> Vector2;
typedef struct Vector3_s<float> Vector3;
typedef struct Vector4_s<float> Vector4;

typedef struct Vector2_s<double> DVector2;
typedef struct Vector3_s<double> DVector3;
typedef struct Vector4_s<double> DVector4;





typedef struct Vector2_s<int> IVector2;
typedef struct Vector2_s<int8_t> I8Vector2;
typedef struct Vector2_s<int16_t> I16Vector2;
typedef struct Vector2_s<int16_t> I16Vector2;
typedef struct Vector2_s<int32_t> I32Vector2;
typedef struct Vector2_s<int64_t> I64Vector2;


typedef struct Vector3_s<int> IVector3;
typedef struct Vector3_s<int8_t> I8Vector3;
typedef struct Vector3_s<int16_t> I16Vector3;
typedef struct Vector3_s<int32_t> I32Vector3;
typedef struct Vector3_s<int64_t> I64Vector3;


typedef struct Vector4_s<int> IVector4;
typedef struct Vector4_s<int8_t> I8Vector4;
typedef struct Vector4_s<int16_t> I16Vector4;
typedef struct Vector4_s<int32_t> I32Vector4;
typedef struct Vector4_s<int64_t> I64Vector4;





typedef struct Vector2_s<unsigned int> UIVector2;
typedef struct Vector2_s<uint8_t> UI8Vector2;
typedef struct Vector2_s<uint16_t> UI16Vector2;
typedef struct Vector2_s<uint32_t> UI32Vector2;
typedef struct Vector2_s<uint64_t> UI64Vector2;


typedef struct Vector3_s<unsigned int> UIVector3;
typedef struct Vector3_s<uint8_t> UI8Vector3;
typedef struct Vector3_s<uint16_t> UI16Vector3;
typedef struct Vector3_s<uint32_t> UI32Vector3;
typedef struct Vector3_s<uint64_t> UI64Vector3;


typedef struct Vector4_s<unsigned int> UIVector4;
typedef struct Vector4_s<uint8_t> UI8Vector4;
typedef struct Vector4_s<uint16_t> UI16Vector4;
typedef struct Vector4_s<uint32_t> UI32Vector4;
typedef struct Vector4_s<uint64_t> UI64Vector4;





typedef Vector3 Point;
typedef Vector3 Color;
typedef Vector4 ColorA;


} //namespace pPack

#endif