#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


#include <jessy/jessy.hpp>


TEST_SUITE("jessy") {
    
    SCENARIO("empty string") {
        auto const maybe_doc = jessy::document::read("");
        REQUIRE(!maybe_doc);
    }
    
    
    SCENARIO("null") {
        auto const maybe_doc = jessy::document::read("null");
        REQUIRE(maybe_doc);
        REQUIRE(maybe_doc->root.is_null());
        REQUIRE_EQ(maybe_doc->root.type(), jessy::value_type::null);
    }
    
    
    SCENARIO("object") {
        auto const maybe_doc = jessy::document::read("{}");
        REQUIRE(maybe_doc);
        auto const root = maybe_doc->root;
        REQUIRE(root.is_object());
        REQUIRE_EQ(root.type(), jessy::value_type::object);
        auto const y_val = root["y"];
        REQUIRE(y_val.is_null());
    }
    
    SCENARIO("int") {
        auto const maybe_doc = jessy::document::read("{ \"x\": 42, \"y\": \"\" }");
        REQUIRE(maybe_doc);
        auto const x_val = maybe_doc->root["x"];        
        REQUIRE_EQ(x_val.type(), jessy::value_type::uinteger);
        auto const maybe_x = x_val.as_int();
        REQUIRE(maybe_x);
        REQUIRE_EQ(*maybe_x, 42);
        auto const y_val = maybe_doc->root["y"];
        REQUIRE_EQ(y_val.type(), jessy::value_type::string);
        auto const maybe_y = y_val.as_int();
        REQUIRE(!maybe_y);
    }
    
    
    SCENARIO("double") {
        auto const maybe_doc = jessy::document::read("{ \"x\": 42.3 }");
        REQUIRE(maybe_doc);
        auto const x_val = maybe_doc->root["x"];
        REQUIRE_EQ(x_val.type(), jessy::value_type::floating);        
        auto const maybe_x = x_val.as_double();
        REQUIRE(maybe_x);
        REQUIRE_EQ(*maybe_x, 42.3);
    }
    
    
    SCENARIO("array") {
        auto const maybe_doc = jessy::document::read("[ 1, 2, 3 ]");
        REQUIRE(maybe_doc);
        auto sum = 0;
        for(auto const& v: maybe_doc->root) {
            auto const maybe_int = v.as_int();
            if(maybe_int)
                sum += *maybe_int;
        }
        REQUIRE_EQ(sum, 6);
    }
    
}


