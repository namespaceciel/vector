//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// vector& operator=(const vector& c);

#include <cassert>
#include <ciel/vector.hpp>

#include "allocators.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<int, test_allocator<int> > l(3, 2, test_allocator<int>(5));
    ciel::vector<int, test_allocator<int> > l2(l, test_allocator<int>(3));
    l2 = l;
    assert(l2 == l);
    assert(l2.get_allocator() == test_allocator<int>(3));
  }
  {
    ciel::vector<int, other_allocator<int> > l(3, 2, other_allocator<int>(5));
    ciel::vector<int, other_allocator<int> > l2(l, other_allocator<int>(3));
    l2 = l;
    assert(l2 == l);
    assert(l2.get_allocator() == other_allocator<int>(5));
  }
  {
    // Test with Allocator::propagate_on_container_copy_assignment == false_type
    using Alloc = NonPOCCAAllocator<int>;
    bool copy_assigned_into = false;
    ciel::vector<int, Alloc> l(3, 2, Alloc(5, nullptr));
    ciel::vector<int, Alloc> l2(l, Alloc(3, &copy_assigned_into));
    assert(!copy_assigned_into);
    l2 = l;
    assert(!copy_assigned_into);
    assert(l2 == l);
    assert(l2.get_allocator() == Alloc(3, nullptr));
  }
  {
    // Test with Allocator::propagate_on_container_copy_assignment == true_type
    // and equal allocators
    using Alloc = POCCAAllocator<int>;
    bool copy_assigned_into = false;
    ciel::vector<int, Alloc> l(3, 2, Alloc(5, nullptr));
    ciel::vector<int, Alloc> l2(l, Alloc(5, &copy_assigned_into));
    assert(!copy_assigned_into);
    l2 = l;
    assert(copy_assigned_into);
    assert(l2 == l);
    assert(l2.get_allocator() == Alloc(5, nullptr));
  }
  {
    // Test with Allocator::propagate_on_container_copy_assignment == true_type
    // and unequal allocators
    using Alloc = POCCAAllocator<int>;
    bool copy_assigned_into = false;
    ciel::vector<int, Alloc> l(3, 2, Alloc(5, nullptr));
    ciel::vector<int, Alloc> l2(l, Alloc(3, &copy_assigned_into));
    assert(!copy_assigned_into);
    l2 = l;
    assert(copy_assigned_into);
    assert(l2 == l);
    assert(l2.get_allocator() == Alloc(5, nullptr));
  }
  {
    ciel::vector<int, min_allocator<int> > l(3, 2, min_allocator<int>());
    ciel::vector<int, min_allocator<int> > l2(l, min_allocator<int>());
    l2 = l;
    assert(l2 == l);
    assert(l2.get_allocator() == min_allocator<int>());
  }
  {
    ciel::vector<int, safe_allocator<int> > l(3, 2, safe_allocator<int>());
    ciel::vector<int, safe_allocator<int> > l2(l, safe_allocator<int>());
    l2 = l;
    assert(l2 == l);
    assert(l2.get_allocator() == safe_allocator<int>());
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
