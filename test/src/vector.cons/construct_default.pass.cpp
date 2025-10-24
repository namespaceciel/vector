//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// vector();
// vector(const Alloc&);

#include <cassert>
#include <ciel/vector.hpp>

#include "NotConstructible.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

template <class C>
constexpr void test0() {
  static_assert((noexcept(C{})), "");
  static_assert((noexcept(C()) == noexcept(typename C::allocator_type())), "");
  C c;
  assert(c.empty());
  assert(c.get_allocator() == typename C::allocator_type());
  C c1 = {};
  assert(c1.empty());
  assert(c1.get_allocator() == typename C::allocator_type());
}

template <class C>
constexpr void test1(const typename C::allocator_type& a) {
  static_assert((noexcept(C{typename C::allocator_type{}})), "");
  C c(a);
  assert(c.empty());
  assert(c.get_allocator() == a);
}

constexpr bool tests() {
  {
    test0<ciel::vector<int>>();
    test0<ciel::vector<NotConstructible>>();
    test1<ciel::vector<int, test_allocator<int>>>(test_allocator<int>(3));
    test1<ciel::vector<NotConstructible, test_allocator<NotConstructible>>>(test_allocator<NotConstructible>(5));
  }
  {
    ciel::vector<int, limited_allocator<int, 10>> v;
    assert(v.empty());
  }
  {
    test0<ciel::vector<int, min_allocator<int>>>();
    test0<ciel::vector<NotConstructible, min_allocator<NotConstructible>>>();
    test1<ciel::vector<int, min_allocator<int>>>(min_allocator<int>{});
    test1<ciel::vector<NotConstructible, min_allocator<NotConstructible>>>(min_allocator<NotConstructible>{});
  }
  {
    ciel::vector<int, min_allocator<int>> v;
    assert(v.empty());
  }

  {
    test0<ciel::vector<int, explicit_allocator<int>>>();
    test0<ciel::vector<NotConstructible, explicit_allocator<NotConstructible>>>();
    test1<ciel::vector<int, explicit_allocator<int>>>(explicit_allocator<int>{});
    test1<ciel::vector<NotConstructible, explicit_allocator<NotConstructible>>>(explicit_allocator<NotConstructible>{});
  }
  {
    ciel::vector<int, explicit_allocator<int>> v;
    assert(v.empty());
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
