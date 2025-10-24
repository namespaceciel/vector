//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03 && !stdlib=libc++

// <vector>

// vector(vector&& c);

#include <cassert>
#include <ciel/vector.hpp>

#include "MoveOnly.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  test_allocator_statistics alloc_stats;
  {
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > l(test_allocator<MoveOnly>(5, &alloc_stats));
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > lo(test_allocator<MoveOnly>(5, &alloc_stats));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, test_allocator<MoveOnly> > l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }
  {
    ciel::vector<MoveOnly, other_allocator<MoveOnly> > l(other_allocator<MoveOnly>(5));
    ciel::vector<MoveOnly, other_allocator<MoveOnly> > lo(other_allocator<MoveOnly>(5));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, other_allocator<MoveOnly> > l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }
  {
    int a1[] = {1, 3, 7, 9, 10};
    ciel::vector<int> c1(a1, a1 + sizeof(a1) / sizeof(a1[0]));
    ciel::vector<int>::const_iterator i = c1.begin();
    ciel::vector<int> c2 = std::move(c1);
    ciel::vector<int>::iterator j = c2.erase(i);
    assert(*j == 3);
  }
  {
    ciel::vector<MoveOnly, min_allocator<MoveOnly> > l((min_allocator<MoveOnly>()));
    ciel::vector<MoveOnly, min_allocator<MoveOnly> > lo((min_allocator<MoveOnly>()));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, min_allocator<MoveOnly> > l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }
  {
    int a1[] = {1, 3, 7, 9, 10};
    ciel::vector<int, min_allocator<int> > c1(a1, a1 + sizeof(a1) / sizeof(a1[0]));
    ciel::vector<int, min_allocator<int> >::const_iterator i = c1.begin();
    ciel::vector<int, min_allocator<int> > c2 = std::move(c1);
    ciel::vector<int, min_allocator<int> >::iterator j = c2.erase(i);
    assert(*j == 3);
  }
  {
    ciel::vector<MoveOnly, safe_allocator<MoveOnly> > l((safe_allocator<MoveOnly>()));
    ciel::vector<MoveOnly, safe_allocator<MoveOnly> > lo((safe_allocator<MoveOnly>()));
    for (int i = 1; i <= 3; ++i) {
      l.push_back(i);
      lo.push_back(i);
    }
    ciel::vector<MoveOnly, safe_allocator<MoveOnly> > l2 = std::move(l);
    assert(l2 == lo);
    assert(l.empty());
    assert(l2.get_allocator() == lo.get_allocator());
  }
  {
    int a1[] = {1, 3, 7, 9, 10};
    ciel::vector<int, safe_allocator<int> > c1(a1, a1 + sizeof(a1) / sizeof(a1[0]));
    ciel::vector<int, safe_allocator<int> >::const_iterator i = c1.begin();
    ciel::vector<int, safe_allocator<int> > c2 = std::move(c1);
    ciel::vector<int, safe_allocator<int> >::iterator j = c2.erase(i);
    assert(*j == 3);
  }
  {
    alloc_stats.clear();
    using Vect = ciel::vector<int, test_allocator<int> >;
    Vect v(test_allocator<int>(42, 101, &alloc_stats));
    assert(alloc_stats.count == 1);
    assert(alloc_stats.copied == 1);
    assert(alloc_stats.moved == 0);
    {
      const test_allocator<int>& a = v.get_allocator();
      assert(a.get_data() == 42);
      assert(a.get_id() == 101);
    }
    assert(alloc_stats.count == 1);
    alloc_stats.clear_ctor_counters();

    Vect v2 = std::move(v);
    assert(alloc_stats.count == 2);
    assert(alloc_stats.copied == 0);
    assert(alloc_stats.moved == 1);
    {
      const test_allocator<int>& a1 = v.get_allocator();
      assert(a1.get_id() == test_alloc_base::moved_value);
      assert(a1.get_data() == 42);

      const test_allocator<int>& a2 = v2.get_allocator();
      assert(a2.get_id() == 101);
      assert(a2.get_data() == 42);

      assert(a1 == a2);
    }
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
