//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// vector(const vector& v);

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

template <class C>
constexpr void test(const C& x) {
  typename C::size_type s = x.size();
  C c(x);
  assert(c.size() == s);
  assert(c == x);
}

constexpr bool tests() {
  {
    int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 1, 0};
    int* an = a + sizeof(a) / sizeof(a[0]);
    test(ciel::vector<int>(a, an));
  }
  {
    ciel::vector<int, test_allocator<int>> v(3, 2, test_allocator<int>(5));
    ciel::vector<int, test_allocator<int>> v2 = v;
    assert(v2 == v);
    assert(v2.get_allocator() == v.get_allocator());
  }
  {
    // Test copy ctor with empty source
    ciel::vector<int, test_allocator<int>> v(test_allocator<int>(5));
    ciel::vector<int, test_allocator<int>> v2 = v;
    assert(v2 == v);
    assert(v2.get_allocator() == v.get_allocator());
    assert(v2.empty());
  }
  {
    ciel::vector<int, other_allocator<int>> v(3, 2, other_allocator<int>(5));
    ciel::vector<int, other_allocator<int>> v2 = v;
    assert(v2 == v);
    assert(v2.get_allocator() == other_allocator<int>(-2));
  }
  {
    int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 1, 0};
    int* an = a + sizeof(a) / sizeof(a[0]);
    test(ciel::vector<int, min_allocator<int>>(a, an));
    test(ciel::vector<int, safe_allocator<int>>(a, an));
  }
  {
    ciel::vector<int, min_allocator<int>> v(3, 2, min_allocator<int>());
    ciel::vector<int, min_allocator<int>> v2 = v;
    assert(v2 == v);
    assert(v2.get_allocator() == v.get_allocator());
  }
  {
    ciel::vector<int, safe_allocator<int>> v(3, 2, safe_allocator<int>());
    ciel::vector<int, safe_allocator<int>> v2 = v;
    assert(v2 == v);
    assert(v2.get_allocator() == v.get_allocator());
  }

  return true;
}

void test_copy_from_volatile_src() {
  volatile int src[] = {1, 2, 3};
  ciel::vector<int> v(src, src + 3);
  assert(v[0] == 1);
  assert(v[1] == 2);
  assert(v[2] == 3);
}

int main(int, char**) {
  tests();
  static_assert(tests());
  test_copy_from_volatile_src();
  return 0;
}
