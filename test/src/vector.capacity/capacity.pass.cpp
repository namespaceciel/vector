//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// size_type capacity() const;

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<int> v;
    assert(v.capacity() == 0);
  }
  {
    ciel::vector<int> v(100);
    assert(v.capacity() == 100);
    v.push_back(0);
    assert(v.capacity() > 101);
  }
  {
    ciel::vector<int, min_allocator<int>> v;
    assert(v.capacity() == 0);
  }
  {
    ciel::vector<int, min_allocator<int>> v(100);
    assert(v.capacity() == 100);
    v.push_back(0);
    assert(v.capacity() > 101);
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
