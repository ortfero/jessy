# jessy
C++20 wrapper over yyjson

## Snippets

### Read attributes

```cpp
auto maybe_document = jessy::document::read("{ a: \"42\", b: 42, c: 42.42, d: true, e:null, d: [1, 2, 3] }");
if(!maybe_document)
    return;

jessy::document const& doc = *maybe_document;

auto const maybe_a = doc.root["a"].as_string();
if(!maybe_a)
    return;
std::string_view const a = *maybe_a;

auto const maybe_b = doc.root["b"].as_int();
if(!maybe_b)
    return;
std::int64_t const b = *maybe_b;

auto const maybe_c = doc.root["c"].as_double();
if(!maybe_c)
    return;
double const c = *maybe_c;

auto const maybe_d = doc.root["d"].as_bool();
if(!maybe_d)
    return;
bool const c = *maybe_d;
        
bool const e_is_null = doc.root["e"].is_null();

auto sum = 0;
for(jessy::value const& v: doc.root["d"]) {
    auto const maybe_each = v.as_int();
    if(maybe_each)
        sum += *maybe_each;
}
```

