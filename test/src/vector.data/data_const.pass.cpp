//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// const_pointer data() const;

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_macros.h"

struct Nasty {
  constexpr Nasty() : i_(0) {}
  constexpr Nasty(int i) : i_(i) {}
  constexpr ~Nasty() {}

  Nasty* operator&() const {
    assert(false);
    return nullptr;
  }
  int i_;
};

constexpr bool tests() {
  {
    const ciel::vector<int> v;
    assert(v.data() == 0);
  }
  {
    const ciel::vector<int> v(100);
    assert(v.data() == std::addressof(v.front()));
  }
  {
    const ciel::vector<Nasty> v(100);
    assert(v.data() == std::addressof(v.front()));
  }
  {
    const ciel::vector<int, min_allocator<int>> v;
    assert(v.data() == 0);
  }
  {
    const ciel::vector<int, min_allocator<int>> v(100);
    assert(v.data() == &v.front());
  }
  {
    const ciel::vector<Nasty, min_allocator<Nasty>> v(100);
    assert(v.data() == std::addressof(v.front()));
  }
  {
    const ciel::vector<int, safe_allocator<int>> v;
    assert(v.data() == 0);
  }
  {
    const ciel::vector<int, safe_allocator<int>> v(100);
    assert(v.data() == &v.front());
  }
  {
    const ciel::vector<Nasty, safe_allocator<Nasty>> v(100);
    assert(v.data() == std::addressof(v.front()));
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
