# jessy
C++17 header-only library to parse JSON documents.
Based on idea from [yyjson](https://github.com/ibireme/yyjson).

## Snippets

### Read attributes

```cpp
#include <jessy/jessy.hpp>

jessy::parser p = jessy::parser{16, 1024}; // preallocate for 16 values and 1024 chars buffer
// jessy::parser p = jessy::parser{} // do not preallocate buffers
jessy::result const parsed = p.parse("{ a: \"42\", b: 42, c: 42.42, d: true, e:null, f: [1, 2, 3] }");
if(parsed != jessy::result::ok) {
	std::cout << entitle(parsed) << '\n';
    return;
}

std::optional<jessy::value::object> const maybe_root = p.root()->as_object();
assert(maybe_root);
auto const& root = *maybe_root;

jessy::value::object::const_iterator a_it = root.find(root.begin(), "a");
assert(a_it != root.end());
std::optional<std::string_view> maybe_a = a_it->as_string();
assert(maybe_a);
std::string_view const a = *maybe_a;
assert(a == "42");

auto b_it = root.find(++a_it, "b");
assert(b_it != root.end());
std::optional<std::int64_t> maybe_b = b_it->as_int();
assert(maybe_b);
std::int64_t const b = *maybe_b;
assert(b == 42);

auto c_it = root.find(++b_it, "c");
assert(c_it != root.end());
std::optional<double> maybe_c = c_it->as_double();
assert(maybe_c);
double const c = *maybe_c;
assert(c == 42.42);

auto d_it = root.find(++c_it, "d");
assert(d_it != root.end());
std::optional<bool> maybe_d = d_it->as_bool();
assert(maybe_d);
bool const d = *maybe_d;
assert(d);

auto e_it = root.find(++d_it, "e");
assert(e_it != root.end());
assert(e_it->is_null());

auto f_it = root.find(++e_it, "f");
assert(f_it != root.end());
std::optional<jessy::value::array> maybe_f = f_it->as_array();
assert(maybe_f);
auto const& f = *maybe_f;
std::cout << "f size: " << f.size() << '\n';
auto sum = 0ll;
for(jessy::value const& v: f) {
    auto const maybe_each = v.as_int();
    if(maybe_each)
        sum += *maybe_each;
}
```

