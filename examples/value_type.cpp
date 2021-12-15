#include <rd/generator.hpp>
#include <array>
#include <cstdio>
#include <string>
#include <tuple>
#include <vector>

/*
* These examples show different usages of reference/value_type
* template parameters
*
* reference = std::string_view
* value_type = std::string_view
*
* Use this when the strings are string-literals and won't be invalidated
* by incrementing the iterator.
*/

static rd::generator<std::string_view> string_views() {
  co_yield "foo";
  co_yield "bar";
}

/*
* reference = std::string_view
* value_type = std::string
*
* Use this when the string_view would be invalidated by incrementing
* the iterator.
*/
template<typename Allocator>
static rd::generator<std::string_view, std::string>
  strings(std::allocator_arg_t, Allocator a) {
  co_yield {};
  co_yield "start";
  std::string s;
  for (auto sv : string_views()) {
    s = sv;
    s.push_back('!');
    co_yield s;
  }
  co_yield "end";
}

// Resulting vector is deduced using ::value_type.
template<std::ranges::range R>
static std::vector<std::ranges::range_value_t<R>> to_vector(R &&r) {
  std::vector<std::ranges::range_value_t<R>> v;
  for (decltype(auto) x : r) { v.emplace_back(static_cast<decltype(x) &&>(x)); }
  return v;
}

/*
* zip() algorithm produces a generator of tuples where
* the reference type is a tuple of references and
* the value type is a tuple of values.
*/
template<std::ranges::range... Rs, std::size_t... Indices>
static rd::generator<std::tuple<std::ranges::range_reference_t<Rs>...>,
  std::tuple<std::ranges::range_value_t<Rs>...>>
  zip_impl(std::index_sequence<Indices...>, Rs... rs) {
  std::tuple<std::ranges::iterator_t<Rs>...> its{ std::ranges::begin(rs)... };
  std::tuple<std::ranges::sentinel_t<Rs>...> itEnds{ std::ranges::end(rs)... };
  while (true) {
    const bool anyAtEnd =
      ((std::get<Indices>(its) == std::get<Indices>(itEnds)) || ...);
    if (anyAtEnd) break;
    co_yield { *std::get<Indices>(its)... };
    (++std::get<Indices>(its), ...);
  }
}

template<std::ranges::range... Rs>
static rd::generator<std::tuple<std::ranges::range_reference_t<Rs>...>,
  std::tuple<std::ranges::range_value_t<Rs>...>>
  zip(Rs &&...rs) {
  return zip_impl(std::index_sequence_for<Rs...>{}, std::forward<Rs>(rs)...);
}

static void print_string_view(std::string_view sv) {
  for (auto c : sv) std::putc(c, stdout);
  std::putc('\n', stdout);
}

static void value_type_example() {
  std::vector<std::string_view> s1 = to_vector(string_views());
  for (auto &s : s1) print_string_view(s);

  std::printf("\n");

  std::vector<std::string> s2 =
    to_vector(strings(std::allocator_arg, std::allocator<std::byte>{}));
  for (auto &s : s2) { std::printf("%s\n", s.c_str()); }

  std::printf("\n");

  std::vector<std::tuple<std::string, std::string>> s3 =
    to_vector(zip(strings(std::allocator_arg, std::allocator<std::byte>{}),
      strings(std::allocator_arg, std::allocator<std::byte>{})));
  for (auto &[a, b] : s3) { std::printf("%s, %s\n", a.c_str(), b.c_str()); }
}

int main(){
  setbuf(stdout, NULL);
  std::printf("\nvalue_type example\n");
  value_type_example();
}
