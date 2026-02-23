#pragma once
#include <cmath>
#include <sstream>
#include <string>

namespace wsce::math {

// Small, explicit vector type (no hidden allocations, trivially copyable).
struct Vec3 {
  double x{0}, y{0}, z{0};

  Vec3() = default;
  constexpr Vec3(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

  constexpr Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
  constexpr Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
  constexpr Vec3 operator*(double s) const { return {x*s, y*s, z*s}; }
  constexpr Vec3 operator/(double s) const { return {x/s, y/s, z/s}; }

  Vec3& operator+=(const Vec3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
  Vec3& operator-=(const Vec3& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }

  double dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
  Vec3 cross(const Vec3& o) const {
    return { y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x };
  }
  double norm() const { return std::sqrt(dot(*this)); }
  Vec3 normalized() const {
    const double n = norm();
    return (n > 1e-12) ? (*this / n) : Vec3{};
  }

  std::string str() const {
    std::ostringstream oss;
    oss << "(" << x << "," << y << "," << z << ")";
    return oss.str();
  }
};

inline Vec3 operator*(double s, const Vec3& v) { return v*s; }

} // namespace wsce::math