//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// iterator insert(const_iterator position, const value_type& x);

#include <cassert>
#include <ciel/vector.hpp>
#include <cstddef>

#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

constexpr bool test() {
  {
    ciel::vector<int> v(100);
    const int lvalue = 1;
    ciel::vector<int>::iterator i = v.insert(v.cbegin() + 10, lvalue);
    assert(v.size() == 101);
    assert(i == v.begin() + 10);
    int j;
    for (j = 0; j < 10; ++j) assert(v[j] == 0);
    assert(v[j] == 1);
    for (++j; j < 101; ++j) assert(v[j] == 0);
  }
  {
    const std::size_t n = 100;
    ciel::vector<int> v(n);
    v.reserve(n + 1);
    const int lvalue = 1;

    // no reallocation expected
    ciel::vector<int>::iterator it = v.insert(v.cbegin() + n, lvalue);

    assert(v.size() == n + 1);
    assert(it == v.begin() + n);
    for (std::size_t i = 0; i < n; ++i) {
      assert(v[i] == 0);
    }
    assert(v[n] == lvalue);
  }
  {
    ciel::vector<int> v(100);
    while (v.size() < v.capacity()) v.push_back(0);  // force reallocation
    std::size_t sz = v.size();
    const int lvalue = 1;
    ciel::vector<int>::iterator i = v.insert(v.cbegin() + 10, lvalue);
    assert(v.size() == sz + 1);
    assert(i == v.begin() + 10);
    std::size_t j;
    for (j = 0; j < 10; ++j) assert(v[j] == 0);
    assert(v[j] == 1);
    for (++j; j < v.size(); ++j) assert(v[j] == 0);
  }
  {
    ciel::vector<int> v(100);
    while (v.size() < v.capacity()) v.push_back(0);
    v.pop_back();
    v.pop_back();  // force no reallocation
    std::size_t sz = v.size();
    const int lvalue = 1;
    ciel::vector<int>::iterator i = v.insert(v.cbegin() + 10, lvalue);
    assert(v.size() == sz + 1);
    assert(i == v.begin() + 10);
    std::size_t j;
    for (j = 0; j < 10; ++j) assert(v[j] == 0);
    assert(v[j] == 1);
    for (++j; j < v.size(); ++j) assert(v[j] == 0);
  }
  {
    ciel::vector<int, limited_allocator<int, 300>> v(100);
    const int lvalue = 1;
    ciel::vector<int, limited_allocator<int, 300>>::iterator i = v.insert(v.cbegin() + 10, lvalue);
    assert(v.size() == 101);
    assert(i == v.begin() + 10);
    int j;
    for (j = 0; j < 10; ++j) assert(v[j] == 0);
    assert(v[j] == 1);
    for (++j; j < 101; ++j) assert(v[j] == 0);
  }
  {
    ciel::vector<int, min_allocator<int>> v(100);
    const int lvalue = 1;
    ciel::vector<int, min_allocator<int>>::iterator i = v.insert(v.cbegin() + 10, lvalue);
    assert(v.size() == 101);
    assert(i == v.begin() + 10);
    int j;
    for (j = 0; j < 10; ++j) assert(v[j] == 0);
    assert(v[j] == 1);
    for (++j; j < 101; ++j) assert(v[j] == 0);
  }
  {
    ciel::vector<int, safe_allocator<int>> v(100);
    const int lvalue = 1;
    ciel::vector<int, safe_allocator<int>>::iterator i = v.insert(v.cbegin() + 10, lvalue);
    assert(v.size() == 101);
    assert(i == v.begin() + 10);
    int j;
    for (j = 0; j < 10; ++j) assert(v[j] == 0);
    assert(v[j] == 1);
    for (++j; j < 101; ++j) assert(v[j] == 0);
  }

  return true;
}

int main(int, char**) {
  test();
  static_assert(test());

  return 0;
}
