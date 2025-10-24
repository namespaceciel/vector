//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void swap(vector& x);

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<int> v1(100);
    ciel::vector<int> v2(200);
    v1.swap(v2);
    assert(v1.size() == 200);
    assert(v1.capacity() == 200);
    assert(v2.size() == 100);
    assert(v2.capacity() == 100);
  }
  {
    ciel::vector<int, min_allocator<int>> v1(100);
    ciel::vector<int, min_allocator<int>> v2(200);
    v1.swap(v2);
    assert(v1.size() == 200);
    assert(v1.capacity() == 200);
    assert(v2.size() == 100);
    assert(v2.capacity() == 100);
  }
  {
    ciel::vector<int, safe_allocator<int>> v1(100);
    ciel::vector<int, safe_allocator<int>> v2(200);
    v1.swap(v2);
    assert(v1.size() == 200);
    assert(v1.capacity() == 200);
    assert(v2.size() == 100);
    assert(v2.capacity() == 100);
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
