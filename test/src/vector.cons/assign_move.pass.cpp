//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03 && !stdlib=libc++

// <vector>

// vector& operator=(vector&& c);

#include <cassert>
#include <ciel/vector.hpp>

#include "MoveOnly.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > l(test_allocator<MoveOnly>(5));
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > lo(test_allocator<MoveOnly>(5));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > l2(test_allocator<MoveOnly>(5));
    l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }
  {
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > l(test_allocator<MoveOnly>(5));
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > lo(test_allocator<MoveOnly>(5));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > l2(test_allocator<MoveOnly>(6));
    l2 = std::move(l);
    assert(l2 == lo);
    assert(!l.empty());
    assert(l2.get_allocator() == test_allocator<MoveOnly>(6));
  }
  {
    ciel::vector<MoveOnly, other_allocator<MoveOnly> > l(other_allocator<MoveOnly>(5));
    ciel::vector<MoveOnly, other_allocator<MoveOnly> > lo(other_allocator<MoveOnly>(5));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, other_allocator<MoveOnly> > l2(other_allocator<MoveOnly>(6));
    l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }
  {
    ciel::vector<MoveOnly, min_allocator<MoveOnly> > l((min_allocator<MoveOnly>()));
    ciel::vector<MoveOnly, min_allocator<MoveOnly> > lo((min_allocator<MoveOnly>()));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, min_allocator<MoveOnly> > l2((min_allocator<MoveOnly>()));
    l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }
  {
    ciel::vector<MoveOnly, safe_allocator<MoveOnly> > l((safe_allocator<MoveOnly>()));
    ciel::vector<MoveOnly, safe_allocator<MoveOnly> > lo((safe_allocator<MoveOnly>()));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, safe_allocator<MoveOnly> > l2((safe_allocator<MoveOnly>()));
    l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
