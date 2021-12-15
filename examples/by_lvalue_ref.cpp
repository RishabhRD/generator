#include <rd/generator.hpp>
#include <array>
#include <cstdio>
#include <string>
#include <tuple>
#include <vector>

/*
*                                  If I co_yield a...
*                           X / X&&  | X&         | const X&
*                        ------------+------------+-----------
* - generator<X>         move + copy | 2x copy    | 2x copy
* - generator<const X&>     ref      |   ref      |   ref
* - generator<X&&>          ref      | ill-formed | ill-formed
* - generator<X&>         ill-formed |   ref      | ill-formed
*/

struct X {
  int id;
  X(int id) : id(id) { std::printf("X::X(%i)\n", id); }
  X(const X &x) : id(x.id) { std::printf("X::X(copy %i)\n", id); }
  X(X &&x) : id(std::exchange(x.id, -1)) { std::printf("X::X(move %i)\n", id); }
  ~X() { std::printf("X::~X(%i)\n", id); }
};

static rd::generator<X &> by_lvalue_ref_example() {
  // co_yield X{1};          // ill-formed: prvalue -> non-const lvalue
  X x{ 2 };
  co_yield x;// OK
  // co_yield std::move(x);  // ill-formed: xvalue -> non-const lvalue
}


int main(){
  setbuf(stdout, NULL);
  std::printf("by_lvalue_ref_example\n");
  for (auto &&x : by_lvalue_ref_example()) { std::printf("-> %i\n", x.id); }
}
