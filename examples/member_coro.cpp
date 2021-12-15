#include <rd/generator.hpp>
#include <array>
#include <cstdio>
#include <string>
#include <tuple>
#include <vector>

struct member_coro {
  rd::generator<int> f() { co_yield 42; }
};

int main(){
  setbuf(stdout, NULL);
  member_coro m;
  std::printf("%d\n", *(m.f().begin()));
}
