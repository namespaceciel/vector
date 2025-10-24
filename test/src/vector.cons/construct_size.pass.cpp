//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// explicit vector(size_type n);
// explicit vector(size_type n, const Allocator& alloc = Allocator());

#include <cassert>
#include <ciel/vector.hpp>

#include "DefaultOnly.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

template <class C>
constexpr void test(typename C::size_type n, typename C::allocator_type const& a = typename C::allocator_type()) {
  (void)a;
  // Test without a custom allocator
  {
    C c(n);
    assert(c.size() == n);
    assert(c.get_allocator() == typename C::allocator_type());
    for (typename C::const_iterator i = c.cbegin(), e = c.cend(); i != e; ++i) assert(*i == typename C::value_type());
  }

  // Test with a custom allocator
  {
    C c(n, a);
    assert(c.size() == n);
    assert(c.get_allocator() == a);
    for (typename C::const_iterator i = c.cbegin(), e = c.cend(); i != e; ++i) assert(*i == typename C::value_type());
  }
}

constexpr bool tests() {
  test<ciel::vector<int>>(0);
  test<ciel::vector<int>>(50);
  test<ciel::vector<int, min_allocator<int>>>(0);
  test<ciel::vector<int, min_allocator<int>>>(50);
  test<ciel::vector<int, safe_allocator<int>>>(0);
  test<ciel::vector<int, safe_allocator<int>>>(50);

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  test<ciel::vector<DefaultOnly>>(0);
  test<ciel::vector<DefaultOnly>>(500);
  assert(DefaultOnly::count == 0);

  test<ciel::vector<DefaultOnly, min_allocator<DefaultOnly>>>(0);
  test<ciel::vector<DefaultOnly, min_allocator<DefaultOnly>>>(500);
  test<ciel::vector<DefaultOnly, safe_allocator<DefaultOnly>>>(0);
  test<ciel::vector<DefaultOnly, safe_allocator<DefaultOnly>>>(500);
  test<ciel::vector<DefaultOnly, test_allocator<DefaultOnly>>>(0, test_allocator<DefaultOnly>(23));
  test<ciel::vector<DefaultOnly, test_allocator<DefaultOnly>>>(100, test_allocator<DefaultOnly>(23));
  assert(DefaultOnly::count == 0);

  return 0;
}
