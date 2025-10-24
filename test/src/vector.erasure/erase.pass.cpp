//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17

// <vector>

// template <class T, class Allocator, class U>
//   typename vector<T, Allocator>::size_type
//   erase(vector<T, Allocator>& c, const U& value);

#include <ciel/vector.hpp>
#include <optional>

#include "min_allocator.h"
#include "test_allocator.h"
#include "test_macros.h"

template <class S, class U>
constexpr void test0(S s, U val, S expected, std::size_t expected_erased_count) {
  ASSERT_SAME_TYPE(typename S::size_type, decltype(std::erase(s, val)));
  assert(expected_erased_count == std::erase(s, val));
  assert(s == expected);
}

template <class S>
constexpr void test() {
  test0(S(), 1, S(), 0);

  test0(S({1}), 1, S(), 1);
  test0(S({1}), 2, S({1}), 0);

  test0(S({1, 2}), 1, S({2}), 1);
  test0(S({1, 2}), 2, S({1}), 1);
  test0(S({1, 2}), 3, S({1, 2}), 0);
  test0(S({1, 1}), 1, S(), 2);
  test0(S({1, 1}), 3, S({1, 1}), 0);

  test0(S({1, 2, 3}), 1, S({2, 3}), 1);
  test0(S({1, 2, 3}), 2, S({1, 3}), 1);
  test0(S({1, 2, 3}), 3, S({1, 2}), 1);
  test0(S({1, 2, 3}), 4, S({1, 2, 3}), 0);

  test0(S({1, 1, 1}), 1, S(), 3);
  test0(S({1, 1, 1}), 2, S({1, 1, 1}), 0);
  test0(S({1, 1, 2}), 1, S({2}), 2);
  test0(S({1, 1, 2}), 2, S({1, 1}), 1);
  test0(S({1, 1, 2}), 3, S({1, 1, 2}), 0);
  test0(S({1, 2, 2}), 1, S({2, 2}), 1);
  test0(S({1, 2, 2}), 2, S({1}), 2);
  test0(S({1, 2, 2}), 3, S({1, 2, 2}), 0);

  //  Test cross-type erasure
  using opt = std::optional<typename S::value_type>;
  test0(S({1, 2, 1}), opt(), S({1, 2, 1}), 0);
  test0(S({1, 2, 1}), opt(1), S({2}), 2);
  test0(S({1, 2, 1}), opt(2), S({1, 1}), 1);
  test0(S({1, 2, 1}), opt(3), S({1, 2, 1}), 0);
}

constexpr bool tests() {
  test<ciel::vector<int>>();
  test<ciel::vector<int, min_allocator<int>>>();
  test<ciel::vector<int, test_allocator<int>>>();
  test<ciel::vector<int, safe_allocator<int>>>();

  test<ciel::vector<long>>();
  test<ciel::vector<double>>();

  return true;
}

int main(int, char**) {
  tests();
  static_assert(tests());
  return 0;
}
