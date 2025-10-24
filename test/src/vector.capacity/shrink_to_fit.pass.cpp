//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void shrink_to_fit();

#include <cassert>
#include <ciel/vector.hpp>

#include "increasing_allocator.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<int> v(100);
    v.push_back(1);
    v.shrink_to_fit();
    assert(v.capacity() == 101);
    assert(v.size() == 101);
  }
  {
    ciel::vector<int, limited_allocator<int, 401>> v(100);
    v.push_back(1);
    v.shrink_to_fit();
    assert(v.capacity() == 101);
    assert(v.size() == 101);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  if (!TEST_IS_CONSTANT_EVALUATED) {
    ciel::vector<int, limited_allocator<int, 400>> v(100);
    v.push_back(1);
    v.shrink_to_fit();
    LIBCPP_ASSERT(v.capacity() == 200);  // assumes libc++'s 2x growth factor
    assert(v.size() == 101);
  }
#endif
  {
    ciel::vector<int, min_allocator<int>> v(100);
    v.push_back(1);
    v.shrink_to_fit();
    assert(v.capacity() == 101);
    assert(v.size() == 101);
  }
  {
    ciel::vector<int, safe_allocator<int>> v(100);
    v.push_back(1);
    v.shrink_to_fit();
    assert(v.capacity() == 101);
    assert(v.size() == 101);
  }

  return true;
}

#if TEST_STD_VER >= 23
// https://llvm.org/PR95161
constexpr bool test_increasing_allocator() {
  ciel::vector<int, increasing_allocator<int>> v;
  v.push_back(1);
  std::size_t capacity = v.capacity();
  v.shrink_to_fit();
  assert(v.capacity() <= capacity);
  assert(v.size() == 1);

  return true;
}
#endif  // TEST_STD_VER >= 23

int main(int, char**) {
  tests();
  static_assert(tests());
#if TEST_STD_VER >= 23
  test_increasing_allocator();
  static_assert(test_increasing_allocator());
#endif

  return 0;
}
