//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// vector& operator=(vector&& c)
//     noexcept(
//          allocator_type::propagate_on_container_move_assignment::value &&
//          is_nothrow_move_assignable<allocator_type>::value);

// This tests a conforming extension

// UNSUPPORTED: c++03

#include <cassert>
#include <ciel/vector.hpp>

#include "MoveOnly.h"
#include "test_allocator.h"
#include "test_macros.h"

template <class T>
struct some_alloc {
  typedef T value_type;
  some_alloc(const some_alloc&);
  void allocate(std::size_t);
};

template <class T>
struct some_alloc2 {
  typedef T value_type;

  some_alloc2() {}
  some_alloc2(const some_alloc2&);
  void allocate(std::size_t);
  void deallocate(void*, unsigned) {}

  typedef std::false_type propagate_on_container_move_assignment;
  typedef std::true_type is_always_equal;
};

template <class T>
struct some_alloc3 {
  typedef T value_type;

  some_alloc3() {}
  some_alloc3(const some_alloc3&);
  void allocate(std::size_t);
  void deallocate(void*, unsigned) {}

  typedef std::false_type propagate_on_container_move_assignment;
  typedef std::false_type is_always_equal;
};

int main(int, char**) {
  {
    typedef ciel::vector<MoveOnly> C;
    static_assert(std::is_nothrow_move_assignable<C>::value, "");
  }
  {
    typedef ciel::vector<MoveOnly, test_allocator<MoveOnly>> C;
    static_assert(!std::is_nothrow_move_assignable<C>::value, "");
  }
  {
    typedef ciel::vector<MoveOnly, other_allocator<MoveOnly>> C;
    static_assert(std::is_nothrow_move_assignable<C>::value, "");
  }
  {
    typedef ciel::vector<MoveOnly, some_alloc<MoveOnly>> C;
    //  In C++17, move assignment for allocators are not allowed to throw
    static_assert(std::is_nothrow_move_assignable<C>::value, "");
  }

  {  // POCMA false, is_always_equal true
    typedef ciel::vector<MoveOnly, some_alloc2<MoveOnly>> C;
    static_assert(std::is_nothrow_move_assignable<C>::value, "");
  }
  {  // POCMA false, is_always_equal false
    typedef ciel::vector<MoveOnly, some_alloc3<MoveOnly>> C;
    static_assert(!std::is_nothrow_move_assignable<C>::value, "");
  }

  return 0;
}
