#include "wsce/math/vec3.hpp"
#include <cassert>

int main() {
  using wsce::math::Vec3;
  Vec3 a{1,2,3}, b{4,5,6};

  auto c = a + b;
  assert(c.x == 5 && c.y == 7 && c.z == 9);

  auto d = a.cross(b);
  assert(d.x == (2*6 - 3*5));
  assert(d.y == (3*4 - 1*6));
  assert(d.z == (1*5 - 2*4));

  assert(a.norm() > 0.0);
  return 0;
}