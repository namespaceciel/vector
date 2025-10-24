//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_CONTAINERS_SEQUENCES_VECTOR_VECTOR_MODIFIERS_COMMON_H
#define TEST_STD_CONTAINERS_SEQUENCES_VECTOR_VECTOR_MODIFIERS_COMMON_H

#include <type_traits>  // for __libcpp_is_trivially_relocatable

#include "test_macros.h"

#ifndef TEST_HAS_NO_EXCEPTIONS
struct Throws {
  Throws() : v_(0) {}
  Throws(int v) : v_(v) {}
  Throws(const Throws& rhs) : v_(rhs.v_) {
    if (sThrows) throw 1;
  }
  Throws(Throws&& rhs) : v_(rhs.v_) {
    if (sThrows) throw 1;
  }
  Throws& operator=(const Throws& rhs) {
    v_ = rhs.v_;
    return *this;
  }
  Throws& operator=(Throws&& rhs) {
    v_ = rhs.v_;
    return *this;
  }
  int v_;
  static bool sThrows;
};

bool Throws::sThrows = false;

struct ThrowingMoveOnly {
  constexpr ThrowingMoveOnly() : value(0), do_throw(false) {}
  constexpr explicit ThrowingMoveOnly(int v) : value(v), do_throw(false) {}
  constexpr explicit ThrowingMoveOnly(int v, bool throw_) : value(v), do_throw(throw_) {}

  ThrowingMoveOnly(const ThrowingMoveOnly& rhs) = delete;
  ThrowingMoveOnly& operator=(const ThrowingMoveOnly&) = delete;

  constexpr ThrowingMoveOnly(ThrowingMoveOnly&& rhs) : value(rhs.value), do_throw(rhs.do_throw) {
    if (do_throw) throw 1;
  }
  constexpr ThrowingMoveOnly& operator=(ThrowingMoveOnly&& rhs) {
    value = rhs.value;
    do_throw = rhs.do_throw;
    if (do_throw) throw 1;
    return *this;
  }

  constexpr friend bool operator==(ThrowingMoveOnly const& lhs, ThrowingMoveOnly const& rhs) {
    return lhs.value == rhs.value;
  }

  int value;
  bool do_throw;
};
#endif  // TEST_HAS_NO_EXCEPTIONS

struct Tracker {
  int copy_assignments = 0;
  int move_assignments = 0;
};

struct TrackedAssignment {
  Tracker* tracker_;
  constexpr explicit TrackedAssignment(Tracker* tracker) : tracker_(tracker) {}

  TrackedAssignment(TrackedAssignment const&) = default;
  TrackedAssignment(TrackedAssignment&&) = default;

  constexpr TrackedAssignment& operator=(TrackedAssignment const&) {
    tracker_->copy_assignments++;
    return *this;
  }
  constexpr TrackedAssignment& operator=(TrackedAssignment&&) {
    tracker_->move_assignments++;
    return *this;
  }
};

struct NonTriviallyRelocatable {
  int value_;
  constexpr NonTriviallyRelocatable() : value_(0) {}
  constexpr explicit NonTriviallyRelocatable(int v) : value_(v) {}
  constexpr NonTriviallyRelocatable(NonTriviallyRelocatable const& other) : value_(other.value_) {}
  constexpr NonTriviallyRelocatable(NonTriviallyRelocatable&& other) : value_(other.value_) {}
  constexpr NonTriviallyRelocatable& operator=(NonTriviallyRelocatable const& other) {
    value_ = other.value_;
    return *this;
  }
  constexpr NonTriviallyRelocatable& operator=(NonTriviallyRelocatable&& other) {
    value_ = other.value_;
    return *this;
  }

  constexpr friend bool operator==(NonTriviallyRelocatable const& a, NonTriviallyRelocatable const& b) {
    return a.value_ == b.value_;
  }
};
LIBCPP_STATIC_ASSERT(!std::__libcpp_is_trivially_relocatable<NonTriviallyRelocatable>::value, "");

#endif  // TEST_STD_CONTAINERS_SEQUENCES_VECTOR_VECTOR_MODIFIERS_COMMON_H
