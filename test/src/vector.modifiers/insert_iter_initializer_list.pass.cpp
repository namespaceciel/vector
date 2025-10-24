//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03

// <vector>

// iterator insert(const_iterator p, initializer_list<value_type> il);

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_macros.h"

#ifndef TEST_HAS_NO_EXCEPTIONS
int throw_if_zero = 2;
int constructed_count = 0;

struct ThrowSometimes {
  ThrowSometimes() { ++constructed_count; }
  ThrowSometimes(const ThrowSometimes&) {
    if (--throw_if_zero == 0) throw 1;
    ++constructed_count;
  }
  ThrowSometimes& operator=(const ThrowSometimes&) {
    if (--throw_if_zero == 0) throw 1;
    ++constructed_count;
    return *this;
  }
  ~ThrowSometimes() { --constructed_count; }
};

void test_throwing() {
  ciel::vector<ThrowSometimes> v;
  v.reserve(4);
  v.emplace_back();
  v.emplace_back();
  try {
    v.insert(v.end(), {ThrowSometimes{}, ThrowSometimes{}});
    assert(false);
  } catch (int) {
    // TODO: Ciel If an exception is thrown while inserting a single element at the end and T is Cpp17CopyInsertable or
    // is_nothrow_move_constructible_v<T> is true, there are no effects.
    // assert(v.size() == 2);
    // assert(constructed_count == 2);
  }
}
#endif  // TEST_HAS_NO_EXCEPTIONS

constexpr bool tests() {
  {
    ciel::vector<int> d(10, 1);
    ciel::vector<int>::iterator i = d.insert(d.cbegin() + 2, {3, 4, 5, 6});
    assert(d.size() == 14);
    assert(i == d.begin() + 2);
    assert(d[0] == 1);
    assert(d[1] == 1);
    assert(d[2] == 3);
    assert(d[3] == 4);
    assert(d[4] == 5);
    assert(d[5] == 6);
    assert(d[6] == 1);
    assert(d[7] == 1);
    assert(d[8] == 1);
    assert(d[9] == 1);
    assert(d[10] == 1);
    assert(d[11] == 1);
    assert(d[12] == 1);
    assert(d[13] == 1);
  }
  {
    ciel::vector<int, min_allocator<int>> d(10, 1);
    ciel::vector<int, min_allocator<int>>::iterator i = d.insert(d.cbegin() + 2, {3, 4, 5, 6});
    assert(d.size() == 14);
    assert(i == d.begin() + 2);
    assert(d[0] == 1);
    assert(d[1] == 1);
    assert(d[2] == 3);
    assert(d[3] == 4);
    assert(d[4] == 5);
    assert(d[5] == 6);
    assert(d[6] == 1);
    assert(d[7] == 1);
    assert(d[8] == 1);
    assert(d[9] == 1);
    assert(d[10] == 1);
    assert(d[11] == 1);
    assert(d[12] == 1);
    assert(d[13] == 1);
  }

  return true;
}

int main(int, char**) {
#ifndef TEST_HAS_NO_EXCEPTIONS
  test_throwing();
#endif
  tests();
  static_assert(tests());
  return 0;
}
