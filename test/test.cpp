#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


#include <jessy/jessy.hpp>


TEST_SUITE("jessy") {
    
    SCENARIO("empty source") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("");
		REQUIRE_EQ(parsed, jessy::result::incomplete_json);
		REQUIRE(parser.root()->is_null());
    }
    
    
    SCENARIO("null") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("null");
        REQUIRE_EQ(parsed, jessy::result::ok);
        REQUIRE(parser.root()->is_null());
    }
	
	
	SCENARIO("false") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("false");
        REQUIRE_EQ(parsed, jessy::result::ok);
		auto const maybe_bool = parser.root()->as_bool();
        REQUIRE(maybe_bool);
		REQUIRE_EQ(*maybe_bool, false);
    }
	
	
	SCENARIO("true") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("true");
        REQUIRE_EQ(parsed, jessy::result::ok);
		auto const maybe_bool = parser.root()->as_bool();
        REQUIRE(maybe_bool);
		REQUIRE_EQ(*maybe_bool, true);
    }
	
	
	SCENARIO("int") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("-314");
		REQUIRE_EQ(parsed, jessy::result::ok);
        auto const maybe_number = parser.root()->as_int();
        REQUIRE(maybe_number);
		REQUIRE_EQ(*maybe_number, -314ll);
    }
    
    
    SCENARIO("double") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("-3.14e+1");
		REQUIRE_EQ(parsed, jessy::result::ok);
        auto const maybe_number = parser.root()->as_double();
        REQUIRE(maybe_number);
		REQUIRE_EQ(*maybe_number, -31.4);
    }
	
	
	SCENARIO("string") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("\"ok\"");
		REQUIRE_EQ(parsed, jessy::result::ok);
        auto const maybe_string = parser.root()->as_string();
        REQUIRE(maybe_string);
		REQUIRE_EQ(*maybe_string, "ok");
    }
	
	SCENARIO("string with escapes") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("\"ok\\r\\n\\b\\f\\u000D\\u000a\"");
		REQUIRE_EQ(parsed, jessy::result::ok);
        auto const maybe_string = parser.root()->as_string();
        REQUIRE(maybe_string);
		REQUIRE_EQ(*maybe_string, "ok\r\n\b\f\r\n");
    }
	
	
	SCENARIO("array") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("[1, 2, 3]");
		REQUIRE_EQ(parsed, jessy::result::ok);
        auto const maybe_array = parser.root()->as_array();
        REQUIRE(maybe_array);
		auto const& array = *maybe_array;
		REQUIRE_EQ(array.size(), 3);
		auto it = array.begin();
		REQUIRE_NE(it, array.end());
		REQUIRE_EQ(*it->as_int(), 1);
		++it;
		REQUIRE_NE(it, array.end());
		REQUIRE_EQ(*it->as_int(), 2);
		++it;
		REQUIRE_NE(it, array.end());
		REQUIRE_EQ(*it->as_int(), 3);
		++it;
		REQUIRE_EQ(it, array.end());
    }
	
	
	SCENARIO("object") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("{\"x\": 1, \"y\":3.14, \"z\":\"ok\"}");
		REQUIRE_EQ(parsed, jessy::result::ok);
        auto const maybe_object = parser.root()->as_object();
        REQUIRE(maybe_object);
		auto const& object = *maybe_object;
		REQUIRE_EQ(object.size(), 3);
		auto it = object.find(object.begin(), "x");
		REQUIRE_NE(it, object.end());
    }
	
	
	SCENARIO("multiline object") {
		auto const text = R"(
		{
			"requestExpirationTime": 12345678,
			"login": 30436013,
			"balance": 9.9,
			"comment": "test balance"
		})";
		auto parser = jessy::parser{};
        auto const parsed = parser.parse(text);
		REQUIRE_EQ(parsed, jessy::result::ok);
	}
	
	
	SCENARIO("find field at end") {
		auto parser = jessy::parser{};
        auto const parsed = parser.parse("{\"x\": 1}");
		REQUIRE_EQ(parsed, jessy::result::ok);
        auto const maybe_object = parser.root()->as_object();
        REQUIRE(maybe_object);
		auto const& object = *maybe_object;
		auto it = object.find(object.end(), "x");
		REQUIRE_NE(it, object.end());
    }


}


