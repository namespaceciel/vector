# ciel::vector

This library is a C++20 single-header drop-in replacement for `std::vector` designed for personal use.

## Installation

Clone the entire repository into your project directory (e.g., into a `third_party` folder). Then, add the following lines to your `CMakeLists.txt`:

```cmake
add_subdirectory(third_party/vector)
target_link_libraries(${PROJECT_NAME} PRIVATE ciel_vector)
```

Alternatively, you can simply copy [vector.hpp](include/ciel/vector.hpp) to a location of your choice.

## Differences from `std::vector`

### 1. We don't provide a specialization of `vector` for `bool`.

### 2. We don't support incomplete types, as the implementation requires verifying whether `T` is trivially copyable throughout the class.

### 3. Support for [Proposal P1144](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p1144r13.html). We provide an `is_trivially_relocatable` trait, which defaults to `std::is_trivially_copyable`. You can partially specialize this trait for specific classes. When it comes to operations like expansions, insertions, and deletions, we will use `memcpy` or `memmove` for trivially relocatable objects.

Most C++ types are trivially relocatable, except for those containing self-references.

```cpp
#include <memory>
#include <ciel/vector>

template<class T>
struct ciel::is_trivially_relocatable<std::unique_ptr<T>> : std::true_type {};
template<class T>
struct ciel::is_trivially_relocatable<std::shared_ptr<T>> : std::true_type {};

struct NotRelocatable {
    unsigned char buffer_[8]{};
    void* p = buffer_; // Points to a member variable, so it can't be trivially relocated.
};
template<>
struct ciel::is_trivially_relocatable<NotRelocatable> : std::false_type {};
```

Sadly, there is no standard method to automatically detect whether a type is trivially relocatable.

Regarding expansions, when a type is trivially relocatable, we can use `memcpy` instead of move/copy constructing it at the new location and destructing it at the old location. This approach allows for a "destructible move," eliminating the need for constructions and destructions.

### 4. Provide `unchecked_emplace_back`, it serves as an `emplace_back` operation that does not check for available space, under the assumption that the container has sufficient capacity. It is the caller's responsibility to ensure that the container has sufficient capacity by calling `reserve` in advance.

```cpp
#include <ciel/vector>

ciel::vector<int> v;
v.reserve(100);
for (int i = 0; i < 100; ++i) {
    v.unchecked_emplace_back(i);
}
```

### 5. Add `std::initializer_list` overloads for `emplace` and `emplace_back`.

```cpp
ciel::vector<ciel::vector<int>> v;
v.emplace_back({1, 2});
v.emplace(v.end(), {3, 4});
```

Since `{...}` has no type and can't be deduced to `std::initializer_list` by the compiler, the default `emplace` and `emplace_back` in `std::vector` do not support this functionality directly without providing these overloads.

### 6. [LWG 526](https://cplusplus.github.io/LWG/issue526) fully supported.

For the following functions, it is safe when the element is from the same vector.

```cpp
v.assign(5, v[0]);
v.insert(v.begin(), 2, v[0]);
v.insert(v.begin(), v[0]);
v.insert(v.begin(), std::move(v[0]));
v.emplace(v.begin(), v[0]);
v.emplace(v.begin(), std::move(v[0]));
v.emplace_back(v[0]);
v.emplace_back(std::move(v[0]));
v.push_back(v[0]);
v.push_back(std::move(v[0]));
v.resize(5, v[0]);
```

### 7. Preallocate sufficient space for member variable initializations.

```cpp
ciel::vector<T> v;
v.reserve(N);
```

equals to

```cpp
ciel::vector<T> v{ciel::reserve_capacity, N};
```

It can be used in member variable initializations.

The behavior is undefined if N == 0.

### 8. `using iterator = pointer;`

Operations like `--end()` is not supported if `pointer` is raw pointer.

### 9. `resize`'s strong exception safety is not provided.

## Benchmark

Benchmark results are available in the GitHub Actions workflows.

Note that benchmark results may vary between different compilers and compiler options. Compiler optimizations and resulting code generation may coincidentally favor one implementation over another, even when they appear visually identical.

## Usage of Third Party Libraries

This library has no dependencies of its own.

Tests are originated from [llvm-project](https://github.com/llvm/llvm-project/tree/main/libcxx/test/std/containers/sequences/vector).

We use [Google Benchmark](https://github.com/google/benchmark) for benchmarking and comparisons.
