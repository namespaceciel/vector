//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// vector(size_type n, const value_type& x);

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

template <class C>
constexpr void test(typename C::size_type n, const typename C::value_type& x) {
  C c(n, x);
  assert(c.size() == n);
  for (typename C::const_iterator i = c.cbegin(), e = c.cend(); i != e; ++i) assert(*i == x);
}

constexpr bool tests() {
  test<ciel::vector<int>>(0, 3);
  test<ciel::vector<int>>(50, 3);
  // Add 1 for implementations that dynamically allocate a container proxy.
  test<ciel::vector<int, limited_allocator<int, 50 + 1>>>(0, 5);
  test<ciel::vector<int, limited_allocator<int, 50 + 1>>>(50, 5);
  test<ciel::vector<int, limited_allocator<int, 50 + 1>>>(50, 5);
  test<ciel::vector<int, min_allocator<int>>>(0, 3);
  test<ciel::vector<int, min_allocator<int>>>(50, 3);
  test<ciel::vector<int, safe_allocator<int>>>(0, 3);
  test<ciel::vector<int, safe_allocator<int>>>(50, 3);

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
