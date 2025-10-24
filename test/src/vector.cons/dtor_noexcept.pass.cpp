//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// ~vector() // implied noexcept;

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
  ~some_alloc() noexcept(false);
  void allocate(std::size_t);
};

constexpr bool tests() {
  {
    typedef ciel::vector<MoveOnly> C;
    static_assert(std::is_nothrow_destructible<C>::value, "");
  }
  {
    typedef ciel::vector<MoveOnly, test_allocator<MoveOnly>> C;
    static_assert(std::is_nothrow_destructible<C>::value, "");
  }
  {
    typedef ciel::vector<MoveOnly, other_allocator<MoveOnly>> C;
    static_assert(std::is_nothrow_destructible<C>::value, "");
  }
#if defined(_LIBCPP_VERSION)
  {
    typedef ciel::vector<MoveOnly, some_alloc<MoveOnly>> C;
    static_assert(!std::is_nothrow_destructible<C>::value, "");
  }
#endif  // _LIBCPP_VERSION

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
