#include <ciel/vector.hpp>

struct T1 {
  T1() {}
  T1(const T1&) {}
  T1& operator=(const T1&) {}
};

struct T2 {
  T2() {}
  T2(const T2&) {}
  T2& operator=(const T2&) {}
};
template <>
struct ciel::is_trivially_relocatable<T2> : std::true_type {};

struct T3 {
  T3() {}
  T3(const T3&) {}
  T3& operator=(const T3&) {}

  using ciel_trivially_relocatable = T3;
};

static_assert(!ciel::is_trivially_relocatable<T1>::value);
static_assert(ciel::is_trivially_relocatable<T2>::value);
static_assert(ciel::is_trivially_relocatable<T3>::value);

static_assert(!ciel::vector<T1>::expand_via_memcpy);
static_assert(!ciel::vector<T1>::move_via_memmove);
static_assert(ciel::vector<T2>::expand_via_memcpy);
static_assert(ciel::vector<T2>::move_via_memmove);
static_assert(ciel::vector<T3>::expand_via_memcpy);
static_assert(ciel::vector<T3>::move_via_memmove);

int main() {}
