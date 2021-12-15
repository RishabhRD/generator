#include <rd/generator.hpp>
#include <array>
#include <cstdio>
#include <string>
#include <tuple>
#include <vector>

template<typename T> struct stateful_allocator {
  using value_type = T;

  int id;

  explicit stateful_allocator(int id) noexcept : id(id) {}

  template<typename U> stateful_allocator(stateful_allocator<U> x) : id(x.id) {}

  T *allocate(std::size_t count) const {
    std::printf("stateful_allocator(%i).allocate(%zu)\n", id, count);
    return static_cast<T *>(
      ::operator new(count * sizeof(T), std::align_val_t(alignof(T))));
  }

  void deallocate(T *ptr, std::size_t count) {
    std::printf("stateful_allocator(%i).deallocate(%zu)\n", id, count);
    ::operator delete(static_cast<void *>(ptr),
      count * sizeof(T),
      std::align_val_t(alignof(T)));
  }
};

rd::generator<int, int, std::allocator<std::byte>> stateless_example() {
  co_yield 42;
}

rd::generator<int, int, std::allocator<std::byte>>
  stateless_example(std::allocator_arg_t, std::allocator<std::byte> alloc) {
  co_yield 42;
}

template<typename Allocator>
rd::generator<int, int, Allocator> stateful_alloc_example(std::allocator_arg_t,
  Allocator alloc) {
  co_yield 42;
}

int main() {
  std::printf("%d\n",
    *(stateful_alloc_example<std::allocator<std::byte>>(std::allocator_arg, {})
        .begin()));
}
