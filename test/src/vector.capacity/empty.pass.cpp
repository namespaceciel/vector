//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// class vector

// bool empty() const noexcept;

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    typedef ciel::vector<int> C;
    C c;
    ASSERT_NOEXCEPT(c.empty());
    assert(c.empty());
    c.push_back(C::value_type(1));
    assert(!c.empty());
    c.clear();
    assert(c.empty());
  }
  {
    typedef ciel::vector<int, min_allocator<int>> C;
    C c;
    ASSERT_NOEXCEPT(c.empty());
    assert(c.empty());
    c.push_back(C::value_type(1));
    assert(!c.empty());
    c.clear();
    assert(c.empty());
  }
  {
    typedef ciel::vector<int, safe_allocator<int>> C;
    C c;
    ASSERT_NOEXCEPT(c.empty());
    assert(c.empty());
    c.push_back(C::value_type(1));
    assert(!c.empty());
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
