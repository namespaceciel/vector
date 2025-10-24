//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03 && !stdlib=libc++

// <vector>

// template <class... Args> reference emplace_back(Args&&... args);
// return type is 'reference' in C++17; 'void' before

#include <cassert>
#include <ciel/vector.hpp>

#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

class A {
  int i_;
  double d_;

 public:
  A(const A&) = delete;
  A& operator=(const A&) = delete;

  constexpr A(int i, double d) : i_(i), d_(d) {}

  constexpr A(A&& a) : i_(a.i_), d_(a.d_) {
    a.i_ = 0;
    a.d_ = 0;
  }

  constexpr A& operator=(A&& a) {
    i_ = a.i_;
    d_ = a.d_;
    a.i_ = 0;
    a.d_ = 0;
    return *this;
  }

  constexpr int geti() const { return i_; }
  constexpr double getd() const { return d_; }
};

constexpr bool tests() {
  {
    ciel::vector<A> c;
    A& r1 = c.emplace_back(2, 3.5);
    assert(c.size() == 1);
    assert(&r1 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    A& r2 = c.emplace_back(3, 4.5);
    assert(c.size() == 2);
    assert(&r2 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    assert(c.back().geti() == 3);
    assert(c.back().getd() == 4.5);
  }
  {
    ciel::vector<A, limited_allocator<A, 4> > c;
    A& r1 = c.emplace_back(2, 3.5);
    assert(c.size() == 1);
    assert(&r1 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    A& r2 = c.emplace_back(3, 4.5);
    assert(c.size() == 2);
    assert(&r2 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    assert(c.back().geti() == 3);
    assert(c.back().getd() == 4.5);
  }
  {
    ciel::vector<A, min_allocator<A> > c;
    A& r1 = c.emplace_back(2, 3.5);
    assert(c.size() == 1);
    assert(&r1 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    A& r2 = c.emplace_back(3, 4.5);
    assert(c.size() == 2);
    assert(&r2 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    assert(c.back().geti() == 3);
    assert(c.back().getd() == 4.5);
  }
  {
    ciel::vector<A, safe_allocator<A> > c;
    A& r1 = c.emplace_back(2, 3.5);
    assert(c.size() == 1);
    assert(&r1 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    A& r2 = c.emplace_back(3, 4.5);
    assert(c.size() == 2);
    assert(&r2 == &c.back());
    assert(c.front().geti() == 2);
    assert(c.front().getd() == 3.5);
    assert(c.back().geti() == 3);
    assert(c.back().getd() == 4.5);
  }
  {
    ciel::vector<Tag_X, TaggingAllocator<Tag_X> > c;
    c.emplace_back();
    assert(c.size() == 1);
    c.emplace_back(1, 2, 3);
    assert(c.size() == 2);
  }

  {  // LWG 2164
    int arr[] = {0, 1, 2, 3, 4};
    int sz = 5;
    ciel::vector<int> c(arr, arr + sz);
    while (c.size() < c.capacity()) c.push_back(sz++);
    c.emplace_back(c.front());
    assert(c.back() == 0);
    for (int i = 0; i < sz; ++i) assert(c[i] == i);
  }
  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
