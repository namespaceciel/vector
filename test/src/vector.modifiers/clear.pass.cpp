//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void clear() noexcept;

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    int a[] = {1, 2, 3};
    ciel::vector<int> c(a, a + 3);
    ASSERT_NOEXCEPT(c.clear());
    c.clear();
    assert(c.empty());
  }
  {
    int a[] = {1, 2, 3};
    ciel::vector<int, min_allocator<int>> c(a, a + 3);
    ASSERT_NOEXCEPT(c.clear());
    c.clear();
    assert(c.empty());
  }
  {
    int a[] = {1, 2, 3};
    ciel::vector<int, safe_allocator<int>> c(a, a + 3);
    ASSERT_NOEXCEPT(c.clear());
    c.clear();
    assert(c.empty());
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
