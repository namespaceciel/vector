//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03 && !stdlib=libc++

// <vector>

// void push_back(value_type&& x);

#include <cassert>
#include <ciel/vector.hpp>
#include <cstddef>

#include "MoveOnly.h"
#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<MoveOnly> c;
    c.push_back(MoveOnly(0));
    assert(c.size() == 1);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(1));
    assert(c.size() == 2);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(2));
    assert(c.size() == 3);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(3));
    assert(c.size() == 4);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(4));
    assert(c.size() == 5);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
  }
  {
    // libc++ needs 15 because it grows by 2x (1 + 2 + 4 + 8).
    // Use 17 for implementations that dynamically allocate a container proxy
    // and grow by 1.5x (1 for proxy + 1 + 2 + 3 + 4 + 6).
    ciel::vector<MoveOnly, limited_allocator<MoveOnly, 17> > c;
    c.push_back(MoveOnly(0));
    assert(c.size() == 1);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(1));
    assert(c.size() == 2);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(2));
    assert(c.size() == 3);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(3));
    assert(c.size() == 4);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(4));
    assert(c.size() == 5);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
  }
  {
    ciel::vector<MoveOnly, min_allocator<MoveOnly> > c;
    c.push_back(MoveOnly(0));
    assert(c.size() == 1);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(1));
    assert(c.size() == 2);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(2));
    assert(c.size() == 3);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(3));
    assert(c.size() == 4);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
    c.push_back(MoveOnly(4));
    assert(c.size() == 5);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == MoveOnly(j));
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
