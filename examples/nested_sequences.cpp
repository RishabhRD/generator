#include <rd/generator.hpp>
#include <array>
#include <cstdio>
#include <string>
#include <tuple>
#include <vector>

static rd::generator<const uint64_t> fib(int max) {
  auto a = 0, b = 1;
  for (auto n = 0; n < max; n++) {
    co_yield b;
    const auto next = a + b;
    a = b, b = next;
  }
}

static rd::generator<int> other_generator(int i, int j) {
  while (i != j) { co_yield i++; }
}

/*
* Demonstrate ability to yield nested sequences
* 
* Need to use rd::elements_of() to trigger yielding elements of nested
* sequence.
* 
* Supports yielding same generator type (with efficient resumption for
* recursive cases)
* 
* Also supports yielding any other range whose elements are convertible to
* the current generator's elements.
*/
static rd::generator<const uint64_t> nested_sequences_example() {
  std::array<const uint64_t, 5> data = { 2, 4, 6, 8, 10 };

  std::printf("yielding elements_of std::array\n");
  co_yield rd::elements_of(data);

  std::printf("yielding elements_of nested rd::generator\n");
  co_yield rd::elements_of(fib(10));

  std::printf("yielding elements_of other kind of generator\n");
  co_yield rd::elements_of(other_generator(5, 8));
}

int main(){
  setbuf(stdout, NULL);
  std::printf("nested_sequences_example\n");
  for (int a : nested_sequences_example()) { std::printf("-> %i\n", a); }
}
