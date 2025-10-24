//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void reserve(size_type n);

#include <cassert>
#include <ciel/vector.hpp>
#include <stdexcept>

#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<int> v;
    v.reserve(10);
    assert(v.capacity() >= 10);
  }
  {
    ciel::vector<int> v(100);
    assert(v.capacity() == 100);
    v.reserve(50);
    assert(v.size() == 100);
    assert(v.capacity() == 100);
    v.reserve(150);
    assert(v.size() == 100);
    assert(v.capacity() == 150);
  }
  {
    // Add 1 for implementations that dynamically allocate a container proxy.
    ciel::vector<int, limited_allocator<int, 250 + 1>> v(100);
    assert(v.capacity() == 100);
    v.reserve(50);
    assert(v.size() == 100);
    assert(v.capacity() == 100);
    v.reserve(150);
    assert(v.size() == 100);
    assert(v.capacity() == 150);
  }
  {
    ciel::vector<int, min_allocator<int>> v;
    v.reserve(10);
    assert(v.capacity() >= 10);
  }
  {
    ciel::vector<int, min_allocator<int>> v(100);
    assert(v.capacity() == 100);
    v.reserve(50);
    assert(v.size() == 100);
    assert(v.capacity() == 100);
    v.reserve(150);
    assert(v.size() == 100);
    assert(v.capacity() == 150);
  }
  {
    ciel::vector<int, safe_allocator<int>> v;
    v.reserve(10);
    assert(v.capacity() >= 10);
  }
  {
    ciel::vector<int, safe_allocator<int>> v(100);
    assert(v.capacity() == 100);
    v.reserve(50);
    assert(v.size() == 100);
    assert(v.capacity() == 100);
    v.reserve(150);
    assert(v.size() == 100);
    assert(v.capacity() == 150);
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
