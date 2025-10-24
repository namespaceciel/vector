//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void resize(size_type sz);

#include <cassert>
#include <ciel/vector.hpp>

#include "MoveOnly.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<int> v(100);
    v.resize(50);
    assert(v.size() == 50);
    assert(v.capacity() == 100);
    v.resize(200);
    assert(v.size() == 200);
    assert(v.capacity() >= 200);
  }
  {
    // Add 1 for implementations that dynamically allocate a container proxy.
    ciel::vector<int, limited_allocator<int, 300 + 1>> v(100);
    v.resize(50);
    assert(v.size() == 50);
    assert(v.capacity() == 100);
    v.resize(200);
    assert(v.size() == 200);
    assert(v.capacity() >= 200);
  }
  {
    ciel::vector<MoveOnly> v(100);
    v.resize(50);
    assert(v.size() == 50);
    assert(v.capacity() == 100);
    v.resize(200);
    assert(v.size() == 200);
    assert(v.capacity() >= 200);
  }
  {
    // Add 1 for implementations that dynamically allocate a container proxy.
    ciel::vector<MoveOnly, limited_allocator<MoveOnly, 300 + 1>> v(100);
    v.resize(50);
    assert(v.size() == 50);
    assert(v.capacity() == 100);
    v.resize(200);
    assert(v.size() == 200);
    assert(v.capacity() >= 200);
  }
  {
    ciel::vector<MoveOnly, min_allocator<MoveOnly>> v(100);
    v.resize(50);
    assert(v.size() == 50);
    assert(v.capacity() == 100);
    v.resize(200);
    assert(v.size() == 200);
    assert(v.capacity() >= 200);
  }
  {
    ciel::vector<int, safe_allocator<int>> v(100);
    v.resize(50);
    assert(v.size() == 50);
    assert(v.capacity() == 100);
    v.resize(200);
    assert(v.size() == 200);
    assert(v.capacity() >= 200);
  }

  return true;
}

int main(int, char**) {
  tests();

  static_assert(tests());

  return 0;
}
