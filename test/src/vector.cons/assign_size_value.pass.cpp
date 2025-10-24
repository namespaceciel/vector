//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void assign(size_type n, const_reference v);

#include <algorithm>
#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_macros.h"

constexpr bool is6(int x) { return x == 6; }

template <typename Vec>
constexpr void test(Vec& v) {
  v.assign(5, 6);
  assert(v.size() == 5);
  assert(std::all_of(v.begin(), v.end(), is6));
}

constexpr bool tests() {
  {
    typedef ciel::vector<int> V;
    V d1;
    V d2;
    d2.reserve(10);  // no reallocation during assign.
    test(d1);
    test(d2);
  }
  {
    ciel::vector<int> vec;
    vec.reserve(32);
    vec.resize(16);  // destruction during assign
    test(vec);
  }
  {
    typedef ciel::vector<int, min_allocator<int>> V;
    V d1;
    V d2;
    d2.reserve(10);  // no reallocation during assign.
    test(d1);
    test(d2);
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
