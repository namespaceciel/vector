//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// template <class InputIt>
// constexpr void assign(InputIt first, InputIt last);

#include <algorithm>
#include <cassert>
#include <ciel/vector.hpp>

#include "container_test_types.h"
#include "emplace_constructible.h"
#include "min_allocator.h"
#include "test_iterators.h"
#include "test_macros.h"

constexpr bool test() {
  int arr1[] = {42};
  int arr2[] = {1, 101, 42};
  {  // Test with new_size > capacity() == 0 for forward_iterator, resulting in reallocation during assign
    using T = EmplaceConstructibleMoveableAndAssignable<int>;
    using It = forward_iterator<int*>;
    {
      ciel::vector<T> v;
      v.assign(It(arr1), It(std::end(arr1)));
      assert(v[0].value == 42);
    }
    {
      ciel::vector<T> v;
      v.assign(It(arr2), It(std::end(arr2)));
      assert(v[0].value == 1);
      assert(v[1].value == 101);
      assert(v[2].value == 42);
    }
  }
  {  // Test with new_size > capacity() == 0 for input_iterator, resulting in reallocation during assign
    using T = EmplaceConstructibleMoveableAndAssignable<int>;
    using It = cpp17_input_iterator<int*>;
    {
      ciel::vector<T> v;
      v.assign(It(arr1), It(std::end(arr1)));
      assert(v[0].copied == 0);
      assert(v[0].value == 42);
    }
    {
      ciel::vector<T> v;
      v.assign(It(arr2), It(std::end(arr2)));
      // assert(v[0].copied == 0);
      assert(v[0].value == 1);
      // assert(v[1].copied == 0);
      assert(v[1].value == 101);
      assert(v[2].copied == 0);
      assert(v[2].value == 42);
    }
  }

  {  // Test with new_size < size() for forward_iterator, resulting in destruction at end during assign
    using T = EmplaceConstructibleMoveableAndAssignable<int>;
    using It = forward_iterator<int*>;
    {
      ciel::vector<T> v;
      v.reserve(5);
      for (std::size_t i = 0; i < v.capacity(); ++i) v.emplace_back(99);
      v.assign(It(arr1), It(std::end(arr1)));
      assert(v.size() == 1);
      assert(v[0].value == 42);
    }
    {
      ciel::vector<T> v;
      v.reserve(5);
      for (std::size_t i = 0; i < v.capacity(); ++i) v.emplace_back(99);
      v.assign(It(arr2), It(std::end(arr2)));
      assert(v.size() == 3);
      assert(v[0].value == 1);
      assert(v[1].value == 101);
      assert(v[2].value == 42);
    }
  }
  {  // Test with new_size < size() for input_iterator, resulting in destruction at end during assign
    using T = EmplaceConstructibleMoveableAndAssignable<int>;
    using It = cpp17_input_iterator<int*>;
    {
      ciel::vector<T> v;
      v.reserve(5);
      for (std::size_t i = 0; i < v.capacity(); ++i) v.emplace_back(99);
      v.assign(It(arr1), It(std::end(arr1)));
      assert(v.size() == 1);
      assert(v[0].value == 42);
    }
    {
      ciel::vector<T> v;
      v.reserve(5);
      for (std::size_t i = 0; i < v.capacity(); ++i) v.emplace_back(99);
      v.assign(It(arr2), It(std::end(arr2)));
      assert(v.size() == 3);
      assert(v[0].value == 1);
      assert(v[1].value == 101);
      assert(v[2].value == 42);
    }
  }

  {  // Test with size() < new_size < capacity() for forward_iterator, resulting in construction at end during assign
    using T = EmplaceConstructibleMoveableAndAssignable<int>;
    using It = forward_iterator<int*>;
    {
      ciel::vector<T> v;
      v.reserve(5);
      v.assign(It(arr1), It(std::end(arr1)));
      assert(v.size() == 1);
      assert(v[0].value == 42);
    }
    {
      ciel::vector<T> v;
      v.reserve(5);
      for (std::size_t i = 0; i < 2; ++i) v.emplace_back(99);
      v.assign(It(arr2), It(std::end(arr2)));
      assert(v.size() == 3);
      assert(v[0].value == 1);
      assert(v[1].value == 101);
      assert(v[2].value == 42);
    }
  }
  {  // Test with size() < new_size < capacity() for input_iterator, resulting in construction at end during assign
    using T = EmplaceConstructibleMoveableAndAssignable<int>;
    using It = cpp17_input_iterator<int*>;
    {
      ciel::vector<T> v;
      v.reserve(5);
      v.assign(It(arr1), It(std::end(arr1)));
      assert(v.size() == 1);
      assert(v[0].value == 42);
    }
    {
      ciel::vector<T> v;
      v.reserve(5);
      for (std::size_t i = 0; i < 2; ++i) v.emplace_back(99);
      v.assign(It(arr2), It(std::end(arr2)));
      assert(v.size() == 3);
      assert(v[0].value == 1);
      assert(v[1].value == 101);
      assert(v[2].value == 42);
    }
  }

  // Test with a number of elements in the source range that is greater than capacity
  {
    typedef forward_iterator<int*> It;

    ciel::vector<int> dst(10);

    std::size_t n = dst.capacity() * 2;
    ciel::vector<int> src(n);

    dst.assign(It(src.data()), It(src.data() + src.size()));
    assert(dst == src);
  }

  return true;
}

int main(int, char**) {
  test();
  static_assert(test());
  return 0;
}
