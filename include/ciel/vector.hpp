#pragma once

#include <algorithm>
#include <cassert>
#include <compare>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

// Inspired by LLVM libc++ and folly's implementation.

namespace ciel {
inline namespace v {

// ==================== __has_builtin ====================

#ifndef __has_builtin
#define __has_builtin(x) false
#endif

// ==================== unreachable ====================

[[noreturn]] inline void unreachable() {
#if defined(__GNUC__) || __has_builtin(__builtin_unreachable)
  __builtin_unreachable();
#elif defined(_MSC_VER)
  __assume(false);
#endif
}

// ==================== CIEL_THROW_EXCEPTION ====================

#ifndef CIEL_THROW_EXCEPTION
#ifdef __cpp_exceptions
#define CIEL_THROW_EXCEPTION(e) throw e
#else
#define CIEL_THROW_EXCEPTION(e) std::terminate()
#endif
#endif

// ==================== allocator_traits ====================

// allocator_has_construct

template <class, class Alloc, class... Args>
struct allocator_has_construct_impl : std::false_type {};

template <class Alloc, class... Args>
struct allocator_has_construct_impl<std::void_t<decltype(std::declval<Alloc>().construct(std::declval<Args>()...))>,
                                    Alloc, Args...> : std::true_type {};

template <class Alloc, class... Args>
struct allocator_has_construct : allocator_has_construct_impl<void, Alloc, Args...> {};

// allocator_has_destroy

template <class Alloc, class Pointer, class = void>
struct allocator_has_destroy : std::false_type {};

template <class Alloc, class Pointer>
struct allocator_has_destroy<Alloc, Pointer,
                             std::void_t<decltype(std::declval<Alloc>().destroy(std::declval<Pointer>()))>>
    : std::true_type {};

// allocator_has_trivial_construct
// allocator_has_trivial_destroy
// Note that Pointer type may not be same as the pointer to typename Alloc::value_type.

template <class Alloc, class Pointer, class... Args>
struct allocator_has_trivial_construct : std::negation<allocator_has_construct<Alloc, Pointer, Args...>> {};

template <class Alloc, class Pointer>
struct allocator_has_trivial_destroy : std::negation<allocator_has_destroy<Alloc, Pointer>> {};

// specializations for std::allocator

template <class T, class Pointer, class... Args>
struct allocator_has_trivial_construct<std::allocator<T>, Pointer, Args...> : std::true_type {};

template <class T, class Pointer>
struct allocator_has_trivial_destroy<std::allocator<T>, Pointer> : std::true_type {};

// ==================== uninitialized_copy ====================

template <class Alloc, class InputIt, class OutputIt>
constexpr void uninitialized_copy(Alloc& alloc, InputIt first, InputIt last, OutputIt& result) {
  using T = std::iterator_traits<InputIt>::value_type;
  using U = std::iterator_traits<OutputIt>::value_type;

  if (!std::is_constant_evaluated()) {
    constexpr bool via_trivial_construct = allocator_has_trivial_construct<
        Alloc, decltype(std::to_address(std::declval<typename std::allocator_traits<Alloc>::pointer>())),
        decltype(*std::declval<InputIt>())>::value;
    if constexpr (via_trivial_construct && std::contiguous_iterator<InputIt> && std::contiguous_iterator<OutputIt> &&
                  std::is_same_v<T, U> && std::is_trivially_copy_constructible_v<T>) {
      if constexpr (std::is_convertible_v<decltype(std::to_address(first)), const void*> &&
                    std::is_convertible_v<decltype(std::to_address(result)), void*>) {
        const size_t count = std::distance(first, last);
        if (count != 0) {
          std::memcpy(std::to_address(result), std::to_address(first), count * sizeof(T));
          result += count;
        }
        return;
      }
    }
  }

  for (; first != last; ++first) {
    std::allocator_traits<Alloc>::construct(alloc, std::to_address(result), *first);
    ++result;
  }
}

// ==================== range_destroyer ====================

// Destroy ranges in destructor for exception handling.
// Note that Allocator should be reference type.
template <class T, class Allocator>
class range_destroyer {
  static_assert(std::is_lvalue_reference_v<Allocator>);

 private:
  using allocator_type = std::remove_reference_t<Allocator>;
  using pointer = std::allocator_traits<allocator_type>::pointer;

  static_assert(std::is_same_v<typename allocator_type::value_type, T>);

  pointer begin_;
  pointer end_;
  Allocator allocator_;

 public:
  constexpr range_destroyer(pointer begin, pointer end, allocator_type& alloc) noexcept
      : begin_{begin}, end_{end}, allocator_{alloc} {}

  range_destroyer(const range_destroyer&) = delete;
  range_destroyer& operator=(const range_destroyer&) = delete;

  constexpr ~range_destroyer() {
    assert(begin_ <= end_);

    for (; begin_ != end_; ++begin_) {
      std::allocator_traits<allocator_type>::destroy(allocator_, std::to_address(begin_));
    }
  }

  constexpr void advance_forward() noexcept { ++end_; }

  constexpr void advance_forward(const ptrdiff_t n) noexcept { end_ += n; }

  constexpr void advance_backward() noexcept { --begin_; }

  constexpr void advance_backward(const ptrdiff_t n) noexcept { begin_ -= n; }

  constexpr void release() noexcept { end_ = begin_; }

};  // class range_destroyer

// ==================== is_trivially_relocatable ====================

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p1144r10.html

template <class T, class = void>
struct is_trivially_relocatable :
#if defined(__cpp_lib_trivially_relocatable)
    std::is_trivially_relocatable<T>
#elif defined(_LIBCPP___TYPE_TRAITS_IS_TRIVIALLY_RELOCATABLE_H)
    std::__libcpp_is_trivially_relocatable<T>
#else
    std::is_trivially_copyable<T>
#endif
{
};

template <class T>
struct is_trivially_relocatable<T, std::enable_if_t<std::is_same_v<T, typename T::ciel_trivially_relocatable>>>
    : std::true_type {};

template <class First, class Second>
struct is_trivially_relocatable<std::pair<First, Second>>
    : std::conjunction<is_trivially_relocatable<First>, is_trivially_relocatable<Second>> {};

template <class... Types>
struct is_trivially_relocatable<std::tuple<Types...>> : std::conjunction<is_trivially_relocatable<Types>...> {};

template <class T>
inline constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<T>::value;

// ==================== move_if_noexcept ====================

template <class T>
#ifdef __cpp_exceptions
std::conditional_t<
    std::conjunction_v<std::negation<std::is_nothrow_move_constructible<T>>, std::is_copy_constructible<T>>, const T&,
    T&&>
#else
T&&
#endif
    constexpr move_if_noexcept(T& x) noexcept {
  return std::move(x);
}

template <class, class>
class vector;

// ==================== split_buffer ====================

template <class T, class AllocatorReference>
class split_buffer {
  static_assert(std::is_lvalue_reference_v<AllocatorReference>,
                "Allocator should be lvalue reference type as being used by vector.");

 public:
  using value_type = T;
  using allocator_type = std::remove_reference_t<AllocatorReference>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = std::allocator_traits<allocator_type>::pointer;
  using const_pointer = std::allocator_traits<allocator_type>::const_pointer;

 private:
  pointer begin_cap_{nullptr};
  pointer begin_{nullptr};
  pointer end_{nullptr};
  pointer end_cap_{nullptr};
  AllocatorReference allocator_ref_;

  friend class vector<value_type, allocator_type>;

  template <class... Args>
  constexpr void construct(pointer p, Args&&... args) {
    std::allocator_traits<allocator_type>::construct(allocator_ref_, std::to_address(p), std::forward<Args>(args)...);
  }

  constexpr void construct_at_end(const size_type n) {
    assert(end_ + n <= end_cap_);

    for (size_type i = 0; i < n; ++i) {
      unchecked_emplace_back();
    }
  }

  constexpr void construct_at_end(const size_type n, const value_type& value) {
    assert(end_ + n <= end_cap_);

    for (size_type i = 0; i < n; ++i) {
      unchecked_emplace_back(value);
    }
  }

  template <std::forward_iterator Iter>
  constexpr void construct_at_end(Iter first, Iter last) {
    ciel::v::uninitialized_copy(allocator_ref_, first, last, end_);
  }

  template <class... Args>
  constexpr void unchecked_emplace_front(Args&&... args) {
    assert(begin_cap_ < begin_);

    construct(begin_ - 1, std::forward<Args>(args)...);
    --begin_;
  }

  template <class... Args>
  constexpr void unchecked_emplace_back(Args&&... args) {
    assert(end_ < end_cap_);

    construct(end_, std::forward<Args>(args)...);
    ++end_;
  }

 public:
  constexpr explicit split_buffer(AllocatorReference alloc, const size_type cap, const size_type offset)
      : allocator_ref_(alloc) {
    assert(cap != 0);
    assert(cap >= offset);

    begin_cap_ = std::allocator_traits<allocator_type>::allocate(allocator_ref_, cap);
    end_cap_ = begin_cap_ + cap;
    begin_ = begin_cap_ + offset;
    end_ = begin_;
  }

  split_buffer(const split_buffer& other) = delete;
  split_buffer& operator=(const split_buffer& other) = delete;

  constexpr ~split_buffer() {
    if (begin_cap_) {
      clear();
      std::allocator_traits<allocator_type>::deallocate(allocator_ref_, begin_cap_, capacity());
    }
  }

  [[nodiscard]] constexpr size_type front_spare() const noexcept {
    assert(begin_cap_ <= begin_);

    return begin_ - begin_cap_;
  }

  [[nodiscard]] constexpr size_type back_spare() const noexcept {
    assert(end_ <= end_cap_);

    return end_cap_ - end_;
  }

  [[nodiscard]] constexpr size_type capacity() const noexcept {
    assert(begin_cap_ < end_cap_);  // capacity should not be zero.

    return end_cap_ - begin_cap_;
  }

  constexpr void clear() noexcept {
    assert(begin_ <= end_);

    for (; begin_ != end_; ++begin_) {
      std::allocator_traits<allocator_type>::destroy(allocator_ref_, std::to_address(begin_));
    }
  }

};  // class split_buffer

// ==================== reserve_capacity ====================

struct reserve_capacity_t {};

inline constexpr reserve_capacity_t reserve_capacity;

// ==================== vector ====================

template <class T, class Allocator = std::allocator<T>>
class vector {
  static_assert(std::is_same_v<typename Allocator::value_type, T>);

 public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::allocator_traits<allocator_type>::size_type;
  using difference_type = std::allocator_traits<allocator_type>::difference_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = std::allocator_traits<allocator_type>::pointer;
  using const_pointer = std::allocator_traits<allocator_type>::const_pointer;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 private:
  template <class... Args>
  static constexpr bool via_trivial_construct =
      allocator_has_trivial_construct<allocator_type, decltype(std::to_address(std::declval<pointer>())),
                                      Args...>::value;
  static constexpr bool via_trivial_destroy =
      allocator_has_trivial_destroy<allocator_type, decltype(std::to_address(std::declval<pointer>()))>::value;

 public:
  static constexpr bool expand_via_memcpy =
      is_trivially_relocatable_v<value_type> &&
      via_trivial_construct<decltype(ciel::v::move_if_noexcept(*std::declval<pointer>()))> && via_trivial_destroy;
  static constexpr bool move_via_memmove = is_trivially_relocatable_v<value_type> &&
                                           via_trivial_construct<decltype(std::move(*std::declval<pointer>()))> &&
                                           via_trivial_destroy;

 private:
  pointer begin_{nullptr};
  pointer end_{nullptr};
  pointer end_cap_{nullptr};
  [[no_unique_address]] allocator_type alloc_;

  // Inspired by folly::fbvector, this constant is to optimize away internal_value's branch
  // to always return false when requirements are satisfied.
  static constexpr bool should_pass_by_value = std::is_trivially_copyable_v<value_type> && sizeof(value_type) <= 16;
  using lvalue = std::conditional_t<should_pass_by_value, value_type, const value_type&>;
  using rvalue = std::conditional_t<should_pass_by_value, value_type, value_type&&>;

  [[nodiscard]] constexpr bool internal_value(const value_type& value, pointer begin) const noexcept {
    if constexpr (should_pass_by_value) {
      return false;
    }

    if (std::is_constant_evaluated()) {
      if (!__builtin_constant_p(std::to_address(begin) <= std::addressof(value) &&
                                std::addressof(value) < std::to_address(end_))) {
        return false;
      }
    }

    if (std::to_address(begin) <= std::addressof(value) && std::addressof(value) < std::to_address(end_)) [[unlikely]] {
      return true;
    }

    return false;
  }

  [[nodiscard]] constexpr size_type recommend_cap(const size_type new_size) const {
    assert(new_size > 0);

    const size_type ms = max_size();

    if (new_size > ms) [[unlikely]] {
      CIEL_THROW_EXCEPTION(std::length_error("ciel::vector expanding size is beyond max_size"));
    }

    const size_type cap = capacity();

    if (cap >= ms / 2) [[unlikely]] {
      return ms;
    }

    return std::max(cap * 2, new_size);
  }

  template <class... Args>
  constexpr void construct(pointer p, Args&&... args) {
    std::allocator_traits<allocator_type>::construct(alloc_, std::to_address(p), std::forward<Args>(args)...);
  }

  constexpr void destroy(pointer p) noexcept {
    assert(begin_ <= p);
    assert(p < end_);

    std::allocator_traits<allocator_type>::destroy(alloc_, std::to_address(p));
  }

  constexpr pointer destroy(pointer first, pointer last) noexcept {
    assert(begin_ <= first);
    assert(first <= last);
    assert(last <= end_);

    const pointer res = first;

    for (; first != last; ++first) {
      std::allocator_traits<allocator_type>::destroy(alloc_, std::to_address(first));
    }

    return res;
  }

  constexpr void construct_at_end(const size_type n) {
    assert(end_ + n <= end_cap_);

    for (size_type i = 0; i < n; ++i) {
      unchecked_emplace_back();
    }
  }

  constexpr void construct_at_end(const size_type n, lvalue value) {
    assert(end_ + n <= end_cap_);

    for (size_type i = 0; i < n; ++i) {
      unchecked_emplace_back(value);
    }
  }

  template <std::forward_iterator Iter>
  constexpr void construct_at_end(Iter first, Iter last) {
    ciel::v::uninitialized_copy(alloc_, first, last, end_);
  }

  constexpr void set_nullptr() noexcept {
    begin_ = nullptr;
    end_ = nullptr;
    end_cap_ = nullptr;
  }

  constexpr void init(const size_type count) {
    assert(count != 0);
    assert(begin_ == nullptr);
    assert(end_ == nullptr);
    assert(end_cap_ == nullptr);

    begin_ = std::allocator_traits<allocator_type>::allocate(alloc_, count);
    end_cap_ = begin_ + count;
    end_ = begin_;
  }

  constexpr void reset() noexcept {
    do_destroy();
    set_nullptr();
  }

  constexpr void reset(const size_type count) {
    assert(count != 0);

    do_destroy();
    set_nullptr();  // It's neccessary since allocation would throw.
    init(count);
  }

  constexpr void swap_out_buffer(split_buffer<value_type, allocator_type&>&& sb) noexcept(
      expand_via_memcpy || std::is_nothrow_move_constructible_v<value_type>) {
    assert(sb.front_spare() == size());

    // If either dest or src is an invalid or null pointer, memcpy's behavior is undefined, even if count is zero.
    if (begin_) {
      if (!std::is_constant_evaluated() && expand_via_memcpy) {
        std::memcpy(std::to_address(sb.begin_cap_), std::to_address(begin_), sizeof(value_type) * size());
        // sb.begin_ = sb.begin_cap_;

      } else {
        for (pointer p = end_; p != begin_;) {
          --p;
          sb.unchecked_emplace_front(ciel::v::move_if_noexcept(*p));
        }

        clear();
      }

      std::allocator_traits<allocator_type>::deallocate(alloc_, begin_, capacity());
    }

    begin_ = sb.begin_cap_;
    end_ = sb.end_;
    end_cap_ = sb.end_cap_;

    sb.begin_cap_ = nullptr;  // enough for split_buffer's destructor
  }

  constexpr void swap_out_buffer(split_buffer<value_type, allocator_type&>&& sb,
                                 pointer pos) noexcept(expand_via_memcpy ||
                                                       std::is_nothrow_move_constructible_v<value_type>) {
    // If either dest or src is an invalid or null pointer, memcpy's behavior is undefined, even if count is zero.
    if (begin_) {
      const size_type front_count = pos - begin_;
      const size_type back_count = end_ - pos;

      assert(sb.front_spare() == front_count);
      assert(sb.back_spare() >= back_count);

      if (!std::is_constant_evaluated() && expand_via_memcpy) {
        std::memcpy(std::to_address(sb.begin_cap_), std::to_address(begin_), sizeof(value_type) * front_count);
        // sb.begin_ = sb.begin_cap_;

        std::memcpy(std::to_address(sb.end_), std::to_address(pos), sizeof(value_type) * back_count);
        sb.end_ += back_count;

      } else {
        for (pointer p = pos; p != begin_;) {
          --p;
          sb.unchecked_emplace_front(ciel::v::move_if_noexcept(*p));
        }

        for (pointer p = pos; p != end_; ++p) {
          sb.unchecked_emplace_back(ciel::v::move_if_noexcept(*p));
        }

        clear();
      }

      std::allocator_traits<allocator_type>::deallocate(alloc_, begin_, capacity());
    }

    begin_ = sb.begin_cap_;
    end_ = sb.end_;
    end_cap_ = sb.end_cap_;

    sb.begin_cap_ = nullptr;  // enough for split_buffer's destructor
  }

  template <class... Args>
  constexpr void emplace_back_aux(Args&&... args) {
    if (end_ == end_cap_) {
      split_buffer<value_type, allocator_type&> sb(alloc_, recommend_cap(size() + 1), size());
      sb.unchecked_emplace_back(std::forward<Args>(args)...);
      swap_out_buffer(std::move(sb));

    } else {
      unchecked_emplace_back(std::forward<Args>(args)...);
    }
  }

  template <class... Args>
  constexpr void unchecked_emplace_back_aux(Args&&... args) {
    assert(end_ < end_cap_);

    construct(end_, std::forward<Args>(args)...);
    ++end_;
  }

  constexpr void do_destroy() noexcept {
    if (begin_) {
      clear();
      std::allocator_traits<allocator_type>::deallocate(alloc_, begin_, capacity());
    }
  }

 public:
  constexpr vector() = default;

  constexpr explicit vector(const allocator_type& alloc) noexcept(std::is_nothrow_copy_constructible_v<allocator_type>)
      : alloc_(alloc) {}

  constexpr explicit vector(const size_type count, const allocator_type& alloc = allocator_type()) : vector(alloc) {
    if (count > 0) [[likely]] {
      init(count);
      construct_at_end(count);
    }
  }

  constexpr vector(const size_type count, const value_type& value, const allocator_type& alloc = allocator_type())
      : vector(alloc) {
    if (count > 0) [[likely]] {
      init(count);
      construct_at_end(count, value);
    }
  }

  template <std::input_iterator Iter>
  constexpr vector(Iter first, Iter last, const allocator_type& alloc = allocator_type()) : vector(alloc) {
    for (; first != last; ++first) {
      emplace_back(*first);
    }
  }

  template <std::forward_iterator Iter>
  constexpr vector(Iter first, Iter last, const allocator_type& alloc = allocator_type()) : vector(alloc) {
    if (const auto count = std::distance(first, last); count > 0) [[likely]] {
      init(count);
      construct_at_end(first, last);
    }
  }

  constexpr vector(const vector& other)
      : vector(other.begin(), other.end(),
               std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.alloc_)) {}

  constexpr vector(vector&& other) noexcept
      : begin_(std::exchange(other.begin_, nullptr)),
        end_(std::exchange(other.end_, nullptr)),
        end_cap_(std::exchange(other.end_cap_, nullptr)),
        alloc_(std::move(other.alloc_)) {}

  constexpr vector(const vector& other, const std::type_identity_t<Allocator>& alloc)
      : vector(other.begin(), other.end(), alloc) {}

  constexpr vector(vector&& other, const std::type_identity_t<Allocator>& alloc) : vector(alloc) {
    if (alloc_ == other.alloc_) {
      begin_ = std::exchange(other.begin_, nullptr);
      end_ = std::exchange(other.end_, nullptr);
      end_cap_ = std::exchange(other.end_cap_, nullptr);

    } else if (other.size() > 0) {
      init(other.size());

      if constexpr (std::is_trivially_copy_constructible_v<value_type> &&
                    std::is_trivially_move_constructible_v<value_type>) {
        // std::move_iterator is not contiguous_iterator
        construct_at_end(other.begin(), other.end());

      } else {
        construct_at_end(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
      }
    }
  }

  constexpr vector(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
      : vector(init.begin(), init.end(), alloc) {}

  constexpr vector(reserve_capacity_t, const size_type count, const allocator_type& alloc = allocator_type())
      : vector(alloc) {
    init(count);
  }

  constexpr ~vector() { do_destroy(); }

  constexpr vector& operator=(const vector& other) {
    if (this == std::addressof(other)) [[unlikely]] {
      return *this;
    }

    if constexpr (std::is_same_v<typename std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment,
                                 std::true_type>) {
      if (alloc_ != other.alloc_) {
        reset();
      }

      alloc_ = other.alloc_;
    }

    assign(other.begin(), other.end(), other.size());

    return *this;
  }

  constexpr vector& operator=(vector&& other) noexcept(
      std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<allocator_type>::is_always_equal::value) {
    if (this == std::addressof(other)) [[unlikely]] {
      return *this;
    }

    if (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value ||
        alloc_ == other.alloc_) {
      swap(other);

    } else if constexpr (std::is_trivially_copyable_v<value_type>) {
      // std::move_iterator is not contiguous_iterator
      assign(other.begin(), other.end(), other.size());

    } else {
      assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()), other.size());
    }

    return *this;
  }

  constexpr void assign(const size_type count, lvalue value) {
    if (capacity() < count) {
      if (internal_value(value, begin_)) {
        const value_type copy = std::move(*(begin_ + (std::addressof(value) - std::to_address(begin_))));
        reset(count);
        construct_at_end(count, copy);

      } else {
        reset(count);
        construct_at_end(count, value);
      }

      return;
    }

    if (count >= size()) {
      std::fill_n(begin_, size(), value);
      construct_at_end(count - size(), value);

    } else {
      std::fill_n(begin_, count, value);
      end_ = destroy(begin_ + count, end_);
    }
  }

 private:
  template <std::forward_iterator Iter>
  constexpr void assign(Iter first, Iter last, const size_type count) {
    assert(std::distance(first, last) == count);

    if (capacity() < count) {
      reset(count);
      construct_at_end(first, last);
      return;
    }

    if (const auto sz = size(); sz > count) {
      auto mid = std::copy(first, last, begin_);
      end_ = destroy(mid, end_);

    } else if (sz < count) {
      auto mid = std::next(first, size());
      std::copy(first, mid, begin_);
      construct_at_end(mid, last);
    }
  }

 public:
  constexpr vector& operator=(std::initializer_list<value_type> ilist) {
    assign(ilist.begin(), ilist.end(), ilist.size());
    return *this;
  }

  template <std::forward_iterator Iter>
  constexpr void assign(Iter first, Iter last) {
    const size_type count = std::distance(first, last);

    assign(first, last, count);
  }

  template <std::input_iterator Iter>
  constexpr void assign(Iter first, Iter last) {
    pointer p = begin_;
    for (; first != last && p != end_; ++first) {
      *p = *first;
      ++p;
    }

    if (p != end_) {
      end_ = destroy(p, end_);

    } else {
      for (; first != last; ++first) {
        emplace_back(*first);
      }
    }
  }

  constexpr void assign(std::initializer_list<value_type> ilist) { assign(ilist.begin(), ilist.end(), ilist.size()); }

  constexpr allocator_type get_allocator() const noexcept { return alloc_; }

  [[nodiscard]] constexpr reference at(const size_type pos) {
    if (pos >= size()) [[unlikely]] {
      CIEL_THROW_EXCEPTION(std::out_of_range("ciel::vector::at pos is not within the range"));
    }

    return begin_[pos];
  }

  [[nodiscard]] constexpr const_reference at(const size_type pos) const {
    if (pos >= size()) [[unlikely]] {
      CIEL_THROW_EXCEPTION(std::out_of_range("ciel::vector::at pos is not within the range"));
    }

    return begin_[pos];
  }

  [[nodiscard]] constexpr reference operator[](const size_type pos) {
    assert(pos < size());

    return begin_[pos];
  }

  [[nodiscard]] constexpr const_reference operator[](const size_type pos) const {
    assert(pos < size());

    return begin_[pos];
  }

  [[nodiscard]] constexpr reference front() {
    assert(!empty());

    return begin_[0];
  }

  [[nodiscard]] constexpr const_reference front() const {
    assert(!empty());

    return begin_[0];
  }

  [[nodiscard]] constexpr reference back() {
    assert(!empty());

    return *(end_ - 1);
  }

  [[nodiscard]] constexpr const_reference back() const {
    assert(!empty());

    return *(end_ - 1);
  }

  [[nodiscard]] constexpr T* data() noexcept { return std::to_address(begin_); }

  [[nodiscard]] constexpr const T* data() const noexcept { return std::to_address(begin_); }

  [[nodiscard]] constexpr iterator begin() noexcept { return {begin_}; }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return {begin_}; }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

  [[nodiscard]] constexpr iterator end() noexcept { return {end_}; }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return {end_}; }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return begin_ == end_; }

  [[nodiscard]] constexpr size_type size() const noexcept { return end_ - begin_; }

  [[nodiscard]] constexpr size_type max_size() const noexcept {
    return std::min<size_type>(std::numeric_limits<difference_type>::max(),
                               std::allocator_traits<allocator_type>::max_size(alloc_));
  }

  constexpr void reserve(const size_type new_cap) {
    if (new_cap <= capacity()) {
      return;
    }

    if (new_cap > max_size()) [[unlikely]] {
      CIEL_THROW_EXCEPTION(std::length_error{"ciel::vector::reserve capacity beyond max_size"});
    }

    split_buffer<value_type, allocator_type&> sb(alloc_, new_cap, size());
    swap_out_buffer(std::move(sb));
  }

  [[nodiscard]] constexpr size_type capacity() const noexcept { return end_cap_ - begin_; }

  constexpr void shrink_to_fit() {
    if (size() == capacity()) [[unlikely]] {
      return;
    }

    if (size() > 0) {
#ifdef __cpp_exceptions
      try {
#endif
        split_buffer<value_type, allocator_type&> sb(alloc_, size(), size());
        swap_out_buffer(std::move(sb));
#ifdef __cpp_exceptions
      } catch (...) {
      }
#endif
    } else {
      std::allocator_traits<allocator_type>::deallocate(alloc_, begin_, capacity());
      set_nullptr();
    }
  }

  constexpr void clear() noexcept { end_ = destroy(begin_, end_); }

 private:
  template <class ExpansionCallback, class AppendCallback, class InsertCallback, class IsInternalValueCallback>
  constexpr iterator insert_impl(pointer pos, const size_type count, ExpansionCallback&& expansion_callback,
                                 AppendCallback&& append_callback, InsertCallback&& insert_callback,
                                 IsInternalValueCallback&& is_internal_value_callback) {
    assert(begin_ <= pos);
    assert(pos <= end_);
    assert(count != 0);

    const size_type pos_index = pos - begin_;

    if (size() + count > capacity()) {  // expansion
      split_buffer<value_type, allocator_type&> sb(alloc_, recommend_cap(size() + count), pos_index);
      expansion_callback(sb);
      swap_out_buffer(std::move(sb), pos);

    } else if (pos == end_) {  // equal to emplace_back
      append_callback();

    } else {
      const bool is_internal_value = is_internal_value_callback();
      const pointer old_end = end_;

      range_destroyer<value_type, allocator_type&> rd{end_ + count, end_ + count, alloc_};
      // ------------------------------------
      // begin                  pos       end
      //                       ----------
      //                       first last
      //                       |  count |
      // relocate [pos, end) count units later
      // ----------------------          --------------
      // begin             new_end       pos    |   end
      //                       ----------       |
      //                       first last      range_destroyer in case of exceptions
      //                       |  count |
      if (!std::is_constant_evaluated() && move_via_memmove) {
        const size_type pos_end_dis = end_ - pos;
        std::memmove(std::to_address(pos + count), std::to_address(pos), sizeof(value_type) * pos_end_dis);
        end_ = pos;
        rd.advance_backward(pos_end_dis);

      } else {
        for (pointer p = end_; p != pos;) {
          --p;
          construct(p + count, std::move(*p));
          destroy(p);
          --end_;
          rd.advance_backward();
        }
      }
      // ----------------------------------------------
      // begin             first        last        end
      //                                 pos
      //                               new_end
      if (is_internal_value) {
        insert_callback();

      } else {
        append_callback();
      }
      // new_end
      end_ = old_end + count;
      rd.release();
    }

    return begin() + pos_index;
  }

 public:
  constexpr iterator insert(const_iterator p, lvalue value) {
    const pointer pos = begin_ + (p - begin());

    return insert_impl(
        pos, 1, [&](split_buffer<value_type, allocator_type&>& sb) { sb.unchecked_emplace_back(value); },
        [&] { unchecked_emplace_back(value); }, [&] { unchecked_emplace_back(*(std::addressof(value) + 1)); },
        [&] { return internal_value(value, pos); });
  }

  constexpr iterator insert(const_iterator p, rvalue value)
    requires(!should_pass_by_value)
  {
    const pointer pos = begin_ + (p - begin());

    return insert_impl(
        pos, 1, [&](split_buffer<value_type, allocator_type&>& sb) { sb.unchecked_emplace_back(std::move(value)); },
        [&] { unchecked_emplace_back(std::move(value)); },
        [&] { unchecked_emplace_back(std::move(*(std::addressof(value) + 1))); },
        [&] { return internal_value(value, pos); });
  }

  constexpr iterator insert(const_iterator p, size_type count, lvalue value) {
    const pointer pos = begin_ + (p - begin());

    if (count == 0) [[unlikely]] {
      return {pos};
    }

    return insert_impl(
        pos, count, [&](split_buffer<value_type, allocator_type&>& sb) { sb.construct_at_end(count, value); },
        [&] { construct_at_end(count, value); }, [&] { construct_at_end(count, *(std::addressof(value) + count)); },
        [&] { return internal_value(value, pos); });
  }

 private:
  template <std::forward_iterator Iter>
  constexpr iterator insert(const_iterator p, Iter first, Iter last, size_type count) {
    const pointer pos = begin_ + (p - begin());

    if (count == 0) [[unlikely]] {
      return {pos};
    }

    return insert_impl(
        pos, count, [&](split_buffer<value_type, allocator_type&>& sb) { sb.construct_at_end(first, last); },
        [&] { construct_at_end(first, last); }, [&] { unreachable(); }, [&] { return false; });
  }

 public:
  template <std::forward_iterator Iter>
  constexpr iterator insert(const_iterator pos, Iter first, Iter last) {
    return insert(pos, first, last, std::distance(first, last));
  }

  // Construct them all at the end at first, then rotate them to the right place.
  template <std::input_iterator Iter>
  constexpr iterator insert(const_iterator p, Iter first, Iter last) {
    const pointer pos = begin_ + (p - begin());

    const auto pos_index = pos - begin();
    const size_type old_size = size();

    for (; first != last; ++first) {
      emplace_back(*first);
    }

    std::rotate(begin() + pos_index, begin() + old_size, end());
    return begin() + pos_index;
  }

  constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
    return insert(pos, ilist.begin(), ilist.end(), ilist.size());
  }

  template <class... Args>
  constexpr iterator emplace(const_iterator p, Args&&... args) {
    const pointer pos = begin_ + (p - begin());

    return insert_impl(
        pos, 1,
        [&](split_buffer<value_type, allocator_type&>& sb) { sb.unchecked_emplace_back(std::forward<Args>(args)...); },
        [&] { unchecked_emplace_back(std::forward<Args>(args)...); }, [&] { unreachable(); }, [&] { return false; });
  }

  template <class U, class... Args>
  constexpr iterator emplace(const_iterator p, std::initializer_list<U> il, Args&&... args) {
    const pointer pos = begin_ + (p - begin());

    return insert_impl(
        pos, 1,
        [&](split_buffer<value_type, allocator_type&>& sb) {
          sb.unchecked_emplace_back(il, std::forward<Args>(args)...);
        },
        [&] { unchecked_emplace_back(il, std::forward<Args>(args)...); }, [&] { unreachable(); },
        [&] { return false; });
  }

  template <class U>
    requires std::is_same_v<std::remove_cvref_t<U>, value_type>
  constexpr iterator emplace(const_iterator p, U&& value) {
    return insert(p, std::forward<U>(value));
  }

 private:
  constexpr iterator erase_impl(pointer first, pointer last,
                                const difference_type count) noexcept(move_via_memmove ||
                                                                      std::is_nothrow_move_assignable_v<value_type>) {
    assert(last - first == count);
    assert(count != 0);

    const auto index = first - begin_;
    const auto back_count = end_ - last;

    if (back_count == 0) {
      end_ = destroy(first, end_);

    } else if (!std::is_constant_evaluated() && move_via_memmove) {
      destroy(first, last);
      end_ -= count;

      if (count >= back_count) {
        std::memcpy(std::to_address(first), std::to_address(last), sizeof(value_type) * back_count);

      } else {
        std::memmove(std::to_address(first), std::to_address(last), sizeof(value_type) * back_count);
      }

    } else {
      const pointer new_end = std::move(last, end_, first);
      end_ = destroy(new_end, end_);
    }

    return begin() + index;
  }

 public:
  constexpr iterator erase(const_iterator p) {
    const pointer pos = begin_ + (p - begin());
    assert(begin_ <= pos);
    assert(pos < end_);

    return erase_impl(pos, pos + 1, 1);
  }

  constexpr iterator erase(const_iterator f, const_iterator l) {
    const pointer first = begin_ + (f - begin());
    const pointer last = begin_ + (l - begin());
    assert(begin_ <= first);
    assert(last <= end_);

    const auto count = last - first;

    if (count <= 0) [[unlikely]] {
      return last;
    }

    return erase_impl(first, last, count);
  }

  constexpr void push_back(lvalue value) { emplace_back(value); }

  constexpr void push_back(rvalue value)
    requires(!should_pass_by_value)
  {
    emplace_back(std::move(value));
  }

  template <class... Args>
  constexpr reference emplace_back(Args&&... args) {
    emplace_back_aux(std::forward<Args>(args)...);

    return back();
  }

  template <class U, class... Args>
  constexpr reference emplace_back(std::initializer_list<U> il, Args&&... args) {
    emplace_back_aux(il, std::forward<Args>(args)...);

    return back();
  }

  template <class... Args>
  constexpr reference unchecked_emplace_back(Args&&... args) {
    unchecked_emplace_back_aux(std::forward<Args>(args)...);

    return back();
  }

  template <class U, class... Args>
  constexpr reference unchecked_emplace_back(std::initializer_list<U> il, Args&&... args) {
    unchecked_emplace_back_aux(il, std::forward<Args>(args)...);

    return back();
  }

  constexpr void pop_back() noexcept {
    assert(!empty());

    destroy(end_ - 1);
    --end_;
  }

  constexpr void resize(const size_type count) {
    if (const auto sz = size(); sz > count) {
      end_ = destroy(begin_ + count, end_);

    } else if (sz < count) {
      append(count - sz);
    }
  }

  constexpr void resize(const size_type count, lvalue value) {
    if (const auto sz = size(); sz > count) {
      end_ = destroy(begin_ + count, end_);

    } else if (sz < count) {
      append(count - sz, value);
    }
  }

  constexpr void swap(vector& other) noexcept {
    using std::swap;

    swap(begin_, other.begin_);
    swap(end_, other.end_);
    swap(end_cap_, other.end_cap_);

    if constexpr (std::is_same_v<typename std::allocator_traits<allocator_type>::propagate_on_container_swap,
                                 std::true_type>) {
      swap(alloc_, other.alloc_);
    }
  }

  constexpr void append(const size_type count) {
    if (const auto new_size = size() + count; new_size > capacity()) {
      split_buffer<value_type, allocator_type&> sb(alloc_, recommend_cap(new_size), size());
      sb.construct_at_end(count);
      swap_out_buffer(std::move(sb));

    } else {
      construct_at_end(count);
    }
  }

  constexpr void append(const size_type count, lvalue value) {
    if (const auto new_size = size() + count; new_size > capacity()) {
      split_buffer<value_type, allocator_type&> sb(alloc_, recommend_cap(new_size), size());
      sb.construct_at_end(count, value);
      swap_out_buffer(std::move(sb));

    } else {
      construct_at_end(count, value);
    }
  }

};  // class vector

template <class T, class Allocator>
struct is_trivially_relocatable<vector<T, Allocator>>
    : std::conjunction<
          is_trivially_relocatable<Allocator>,
          is_trivially_relocatable<typename std::allocator_traits<std::remove_reference_t<Allocator>>::pointer>> {};

template <class T, class Alloc>
constexpr bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
concept boolean_testable_impl = std::convertible_to<T, bool>;

template <class T>
concept boolean_testable = boolean_testable_impl<T> && requires(T&& t) {
  { !std::forward<T>(t) } -> boolean_testable_impl;
};

inline constexpr auto synth_three_way = []<class T, class U>(const T& t, const U& u)
  requires requires {
    { t < u } -> boolean_testable;
    { u < t } -> boolean_testable;
  }
{
  if constexpr (std::three_way_comparable_with<T, U>) {
    return t <=> u;
  } else {
    if (t < u) return std::weak_ordering::less;
    if (u < t) return std::weak_ordering::greater;
    return std::weak_ordering::equivalent;
  }
};

template <class T, class U = T>
using synth_three_way_result = decltype(ciel::v::synth_three_way(std::declval<T&>(), std::declval<U&>()));

template <class T, class Alloc>
constexpr ciel::v::synth_three_way_result<T> operator<=>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
  return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                                                ciel::v::synth_three_way);
}

template <class Iter, class Alloc = std::allocator<typename std::iterator_traits<Iter>::value_type>>
vector(Iter, Iter, Alloc = Alloc()) -> vector<typename std::iterator_traits<Iter>::value_type, Alloc>;
template <class T>
vector(size_t, const T&) -> vector<T>;

}  // namespace v
}  // namespace ciel

namespace std {

template <class T, class Alloc>
constexpr void swap(ciel::vector<T, Alloc>& lhs, ciel::vector<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

template <class T, class Alloc, class U>
constexpr ciel::vector<T, Alloc>::size_type erase(ciel::vector<T, Alloc>& c, const U& value) {
  auto it = std::remove(c.begin(), c.end(), value);
  const auto res = std::distance(it, c.end());
  c.erase(it, c.end());
  return res;
}

template <class T, class Alloc, class Pred>
constexpr ciel::vector<T, Alloc>::size_type erase_if(ciel::vector<T, Alloc>& c, Pred pred) {
  auto it = std::remove_if(c.begin(), c.end(), pred);
  const auto res = std::distance(it, c.end());
  c.erase(it, c.end());
  return res;
}

}  // namespace std
