//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Ensure that all the elements in the vector are destroyed, especially when reallocating the internal buffer

// UNSUPPORTED: c++03

#include <algorithm>
#include <array>
#include <cassert>
#include <ciel/vector.hpp>
#include <cstddef>

#include "test_macros.h"

struct DestroyTracker {
  constexpr DestroyTracker(ciel::vector<bool>& vec) : vec_(&vec), index_(vec.size()) { vec.push_back(false); }

  constexpr DestroyTracker(const DestroyTracker& other) : vec_(other.vec_), index_(vec_->size()) {
    vec_->push_back(false);
  }

  constexpr DestroyTracker& operator=(const DestroyTracker&) { return *this; }
  constexpr ~DestroyTracker() { (*vec_)[index_] = true; }

  ciel::vector<bool>* vec_;
  size_t index_;
};

template <class Operation>
constexpr void test(Operation operation) {
  ciel::vector<bool> all_destroyed;

  {
    ciel::vector<DestroyTracker> v;
    for (size_t i = 0; i != 100; ++i) operation(v, all_destroyed);
  }

  assert(std::all_of(all_destroyed.begin(), all_destroyed.end(), [](bool b) { return b; }));
}

constexpr bool test() {
  test([](ciel::vector<DestroyTracker>& vec, ciel::vector<bool>& tracker) { vec.emplace_back(tracker); });
  test([](ciel::vector<DestroyTracker>& vec, ciel::vector<bool>& tracker) { vec.push_back(tracker); });
  test([](ciel::vector<DestroyTracker>& vec, ciel::vector<bool>& tracker) { vec.emplace(vec.begin(), tracker); });
  test([](ciel::vector<DestroyTracker>& vec, ciel::vector<bool>& tracker) { vec.insert(vec.begin(), tracker); });
  test([](ciel::vector<DestroyTracker>& vec, ciel::vector<bool>& tracker) { vec.resize(vec.size() + 1, tracker); });
#if TEST_STD_VER >= 23
  test([](ciel::vector<DestroyTracker>& vec, ciel::vector<bool>& tracker) {
    vec.insert_range(vec.begin(), std::array<DestroyTracker, 2>{tracker, tracker});
  });

  test([](ciel::vector<DestroyTracker>& vec, ciel::vector<bool>& tracker) {
    vec.append_range(std::array<DestroyTracker, 2>{tracker, tracker});
  });
#endif

  return true;
}

int main(int, char**) {
  test();
  // static_assert(test()); // TODO: Ciel
  return 0;
}
