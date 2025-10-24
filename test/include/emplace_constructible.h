#ifndef TEST_SUPPORT_EMPLACE_CONSTRUCTIBLE_H
#define TEST_SUPPORT_EMPLACE_CONSTRUCTIBLE_H

#include "test_macros.h"

template <class T>
struct EmplaceConstructible {
  T value;
  constexpr explicit EmplaceConstructible(T xvalue) : value(xvalue) {}
  EmplaceConstructible(EmplaceConstructible const&) = delete;
};

template <class T>
struct EmplaceConstructibleAndMoveInsertable {
  int copied = 0;
  T value;
  constexpr explicit EmplaceConstructibleAndMoveInsertable(T xvalue) : value(xvalue) {}

  constexpr EmplaceConstructibleAndMoveInsertable(EmplaceConstructibleAndMoveInsertable&& Other)
      : copied(Other.copied + 1), value(std::move(Other.value)) {}
};

template <class T>
struct EmplaceConstructibleAndMoveable {
  int copied = 0;
  int assigned = 0;
  T value;
  constexpr explicit EmplaceConstructibleAndMoveable(T xvalue) noexcept : value(xvalue) {}

  constexpr EmplaceConstructibleAndMoveable(EmplaceConstructibleAndMoveable&& Other) noexcept
      : copied(Other.copied + 1), value(std::move(Other.value)) {}

  constexpr EmplaceConstructibleAndMoveable& operator=(EmplaceConstructibleAndMoveable&& Other) noexcept {
    copied = Other.copied;
    assigned = Other.assigned + 1;
    value = std::move(Other.value);
    return *this;
  }
};

template <class T>
struct EmplaceConstructibleMoveableAndAssignable {
  int copied = 0;
  int assigned = 0;
  T value;
  constexpr explicit EmplaceConstructibleMoveableAndAssignable(T xvalue) noexcept : value(xvalue) {}

  constexpr EmplaceConstructibleMoveableAndAssignable(EmplaceConstructibleMoveableAndAssignable&& Other) noexcept
      : copied(Other.copied + 1), value(std::move(Other.value)) {}

  constexpr EmplaceConstructibleMoveableAndAssignable& operator=(
      EmplaceConstructibleMoveableAndAssignable&& Other) noexcept {
    copied = Other.copied;
    assigned = Other.assigned + 1;
    value = std::move(Other.value);
    return *this;
  }

  constexpr EmplaceConstructibleMoveableAndAssignable& operator=(T xvalue) {
    value = std::move(xvalue);
    ++assigned;
    return *this;
  }
};

#endif  // TEST_SUPPORT_EMPLACE_CONSTRUCTIBLE_H
