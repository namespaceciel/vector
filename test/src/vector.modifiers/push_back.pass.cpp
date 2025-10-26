//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void push_back(const value_type& x);

#include <cassert>
#include <ciel/vector.hpp>
#include <cstddef>

#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool tests() {
  {
    ciel::vector<int> c;
    c.push_back(0);
    assert(c.size() == 1);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(1);
    assert(c.size() == 2);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(2);
    assert(c.size() == 3);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(3);
    assert(c.size() == 4);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(4);
    assert(c.size() == 5);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
  }
  {
    // libc++ needs 15 because it grows by 2x (1 + 2 + 4 + 8).
    // Use 17 for implementations that dynamically allocate a container proxy
    // and grow by 1.5x (1 for proxy + 1 + 2 + 3 + 4 + 6).
    ciel::vector<int, limited_allocator<int, 17>> c;
    c.push_back(0);
    assert(c.size() == 1);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(1);
    assert(c.size() == 2);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(2);
    assert(c.size() == 3);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(3);
    assert(c.size() == 4);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(4);
    assert(c.size() == 5);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
  }
  {
    ciel::vector<int, min_allocator<int>> c;
    c.push_back(0);
    assert(c.size() == 1);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(1);
    assert(c.size() == 2);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(2);
    assert(c.size() == 3);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(3);
    assert(c.size() == 4);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
    c.push_back(4);
    assert(c.size() == 5);
    for (int j = 0; static_cast<std::size_t>(j) < c.size(); ++j) assert(c[j] == j);
  }

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
