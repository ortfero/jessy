// This file is part of jessy library
// Copyright 2023 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <charconv>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>


namespace jessy {


	enum class value_type: std::uint8_t {
		null, boolean, number, string, object, array
	}; // value_type
	
	
	inline constexpr std::string_view entitle(value_type type) noexcept {
		switch(type) {
			case value_type::null:
				return "null";
			case value_type::boolean:
				return "boolean";
			case value_type::number:
				return "number";
			case value_type::string:
				return "string";
			case value_type::object:
				return "object";
			case value_type::array:
				return "array";
			default:
				return "unknown";
		}
	}
	
	
	class value_tag {
		std::size_t tag_{0u};
		
	public:
	
		using size_type = std::size_t;
		
		constexpr value_tag() = default;
		constexpr value_tag(value_tag const&) = default;
		constexpr value_tag& operator = (value_tag const&) = default;
		
		constexpr value_tag(value_type type) noexcept {
			if constexpr(sizeof(size_type) == 4)
				tag_ = size_type(type) << 24;
			else
				tag_ = size_type(type) << 56;
		}
		
		
		constexpr value_tag(value_type type, size_type length) noexcept {
			if constexpr(sizeof(size_type) == 4)
				tag_ = (size_type(type) << 24) | (length & 0x00FFFFFF);
			else
				tag_ = (size_type(type) << 56) | (length & 0x00FFFFFFFFFFFFFF);
		}
		
		
		constexpr value_type type() const noexcept {
			if constexpr(sizeof(size_type) == 4)
				return value_type(tag_ >> 24);
			else
				return value_type(tag_ >> 56);
		}
		
		
		constexpr size_type length() const noexcept {
			if constexpr(sizeof(size_type) == 4)
				return tag_ & 0x00FFFFFF;
			else
				return tag_ & 0x00FFFFFFFFFFFFFF;
		}
		
		
		constexpr void length(size_type n) noexcept {
			if constexpr(sizeof(size_type) == 4)
				tag_ |= (n & 0x00FFFFFF);
			else
				tag_ |= (n & 0x00FFFFFFFFFFFFFF);
		}
		
	}; // value_tag
	

	union value_data {
		
		bool boolean;
		char const* text;
		value_tag::size_type count;
		
		constexpr value_data() noexcept: count{0} { }
		constexpr value_data(bool value) noexcept: boolean{value} { }
		constexpr value_data(char const* text) noexcept: text{text} { }
	}; // value_data
	
	
	class parser;
	class value {
	friend class parser;
	
		value_tag tag_;
		value_data data_;
		
		static const value null;
		
		static value const* next_of(value const* it) noexcept {
			switch(it->tag_.type()) {
				case value_type::array:
				case value_type::object:
					return it + it->data_.count + 1;
				default:
					return it + 1;
			}
		}
		
	public:
	
		using size_type = value_tag::size_type;
	
		class array {
		friend class value;
		
			value_tag::size_type length_;
			value const* begin_;
			value const* end_;
			
		public:
			using size_type = value_tag::size_type;
			
			class const_iterator {
			friend class array;
				value const* it_;
			public:
			
				const_iterator(const_iterator const&) = default;
				const_iterator& operator = (const const_iterator&) = default;
				
				bool operator == (const_iterator const& other) const noexcept {
					return it_ == other.it_;
				}
				
				bool operator != (const_iterator const& other) const noexcept {
					return it_ != other.it_;
				}
				
				value const& operator * () const noexcept {
					return *it_;
				}
				
				value const* operator -> () const noexcept {
					return it_;
				}
				
				const_iterator& operator ++ () noexcept {
					it_ = next_of(it_);
					return *this;
				}
				
				const_iterator operator ++ (int) noexcept {
					auto const me{*this};
					++(*this);
					return me;
				}
					
			private:
			
				explicit const_iterator(value const* it) noexcept
				: it_{it} { }
				
			}; // const_iterator
		
			array(array const&) = default;
			array& operator = (array const&) = default;
			size_type size() const noexcept { return length_; }
			bool empty() const noexcept { return begin_ == end_; }
			
			const_iterator begin() const noexcept {
				return const_iterator{begin_};
			}
			
			const_iterator end() const noexcept {
				return const_iterator{end_};
			}
			
		private:
		
			array(size_type length,
			      value const* begin,
				  value const* end) noexcept
			: length_{length}, begin_{begin}, end_{end} { }
		
		}; // array
		
		
		class object {
		friend class value;
		
			value_tag::size_type length_;
			value const* begin_;
			value const* end_;
			
		public:
			using size_type = value_tag::size_type;
			
			class const_iterator {
			friend class object;
				value const* key_;
				value const* value_;
			public:
			
				const_iterator(const_iterator const&) = default;
				const_iterator& operator = (const const_iterator&) = default;
				
				bool operator == (const_iterator const& other) const noexcept {
					return key_ == other.key_;
				}
				
				bool operator != (const_iterator const& other) const noexcept {
					return key_ != other.key_;
				}
				
				std::string_view key() const noexcept {
					return std::string_view{key_->data_.text,
					                        key_->tag_.length()};
				}
				
				value const& operator * () const noexcept {
					return *value_;
				}
				
				value const* operator -> () const noexcept {
					return value_;
				}
				
				const_iterator& operator ++ () noexcept {
					key_ = next_of(value_);
					value_ = key_ + 1;
					return *this;
				}
				
				const_iterator operator ++ (int) noexcept {
					auto const me{*this};
					++(*this);
					return me;
				}
				
			private:
			
				explicit const_iterator(class value const* it) noexcept
				: key_{it}, value_{it + 1} { }
				
			}; // const_iterator
		
			object(object const&) = default;
			object& operator = (object const&) = default;
			size_type size() const noexcept { return length_; }
			bool empty() const noexcept { return begin_ == end_; }
			
			const_iterator begin() const noexcept {
				return const_iterator{begin_};
			}
			
			const_iterator end() const noexcept {
				return const_iterator{end_};
			}
			
			const_iterator find(const_iterator from, std::string_view name) const noexcept {
				auto const* it = from.key_;
				while(it != end_) {
					auto const key = std::string_view{it->data_.text,
					                                  it->tag_.length()};
					if(key == name)
						return const_iterator{it};
					it = next_of(it + 1);
				}
				it = begin_;
				while(it != from.key_) {
					auto const key = std::string_view{it->data_.text,
					                                  it->tag_.length()};
					if(key == name)
						return const_iterator{it};
					it = next_of(it + 1);
				}
				return const_iterator{end_};
			}
			
		private:
		
			object(size_type length,
			       value const* begin,
				   value const* end) noexcept
			: length_{length}, begin_{begin}, end_{end} { }
		}; // object
	
	
		constexpr value() = default;
		constexpr value(value const&) = default;
		constexpr value& operator = (value const&) = default;
		
		
		constexpr value(bool v)
		: tag_{value_type::boolean}, data_{v} { }
		
		
		constexpr value(value_type type)
		: tag_{type} { }
		
		
		constexpr value(value_type type,
		                char const* text,
						size_type length)
		: tag_{type, length}, data_{text} { }
		
		
		constexpr void members_info(size_type length,
		                            size_type count) {
			tag_.length(length);
			data_.count = count;
		}

		
		constexpr value_type type() const noexcept {
			return tag_.type();
		}
		
		
		constexpr bool is_null() const noexcept {
			return tag_.type() == value_type::null;
		}		
		
		
		std::optional<bool> as_bool() const noexcept {
			if(tag_.type() != value_type::boolean)
				return std::nullopt;
			return {data_.boolean};
		}
		
		
		std::optional<std::int64_t> as_int() const noexcept {
			if(tag_.type() != value_type::number)
				return std::nullopt;
			auto result = 0ll;
			auto const converted = std::from_chars(data_.text,
			                                       data_.text + tag_.length(),
												   result);
			if(converted.ec != std::errc{})
				return std::nullopt;
			return {result};
		}
		
		
		std::optional<std::uint64_t> as_uint() const noexcept {
			if(tag_.type() != value_type::number)
				return std::nullopt;
			auto result = 0ull;
			auto const converted = std::from_chars(data_.text,
			                                       data_.text + tag_.length(),
												   result);
			if(converted.ec != std::errc{})
				return std::nullopt;
			return {result};
		}
		
		
		std::optional<double> as_double() const noexcept {
			if(tag_.type() != value_type::number)
				return std::nullopt;
			auto result = 0.0;
			auto const converted = std::from_chars(data_.text,
			                                       data_.text + tag_.length(),
												   result);
			if(converted.ec != std::errc{})
				return std::nullopt;
			return {result};
		}
		
		
		std::optional<std::string_view> as_string() const noexcept {
			if(tag_.type() != value_type::string)
				return std::nullopt;
			return {std::string_view{data_.text, tag_.length()}};
		}
		
		
		std::optional<array> as_array() const noexcept {
			if(tag_.type() != value_type::array)
				return std::nullopt;
			return {array{tag_.length(), this + 1, this + data_.count + 1}};
		}
		
		
		std::optional<object> as_object() const noexcept {
			if(tag_.type() != value_type::object)
				return std::nullopt;
			return {object{tag_.length(), this + 1, this + data_.count + 1}};
		}
						
	}; // value
	
	
	inline constexpr value value::null;
	
	
	enum result {
		ok,
		incomplete_json,
		illformed_json,
		number_is_out_of_range,
		invalid_number,
		unclosed_string,
		invalid_escape_sequence
	}; // result
	
	
	inline constexpr std::string_view entitle(result r) {
		switch(r) {
			case result::ok:
				return "None";
			case result::incomplete_json:
				return "Incomplete JSON";
			case result::illformed_json:
				return "Illformed JSON";
			case result::number_is_out_of_range:
				return "Number is out of range";
			case result::invalid_number:
				return "Invalid number";
			case result::unclosed_string:
				return "Unclosed string";
			case result::invalid_escape_sequence:
				return "Invalid escape sequence";
			default:
				return "Unknown";			
		}
	}
	
	
	class parser {
		
		std::string buffer_;
		std::vector<value> values_;
		char* cursor_{nullptr};
		
	public:
	
		using size_type = std::size_t;
	
		parser() = default;
		parser(parser const&) = default;
		parser& operator = (parser const&) = default;
		parser(parser&&) = default;
		parser& operator = (parser&&) = default;
		
		
		explicit parser(size_type reserve_tokens, size_type buffer_size) {
			values_.reserve(reserve_tokens);
			buffer_.reserve(buffer_size);
		}
		
		
		void clear() {
			buffer_.clear();
			values_.clear();
			cursor_ = nullptr;
		}
		
		
		result parse(std::string_view text) {
			values_.clear();
			buffer_ = text;
			cursor_ = buffer_.data();
			return parse_value();
		}
		
		
		value const* root() const noexcept {
			if(values_.empty())
				return &value::null;
			return &values_.front();
		}
		
	private:
	
	
		static bool is_digit(char c) noexcept {
			constexpr auto t = true;
			constexpr auto f = false;
			static constexpr bool map[] = {
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,t,t,t,t,t,t,t,t,t,t,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f
			};
			static_assert(sizeof(map) == 256, "Invalid digits map");
			return map[unsigned(c)];
		}
	
	
		result parse_value() {
			switch(skip()) {
				case '{':
					return parse_object();
				case '[':
					return parse_array();
				case '"':
					return parse_string();
				case 'n':
					return parse_null();
				case 't':
					return parse_true();
				case 'f':
					return parse_false();
				case '-':
					if(!is_digit(cursor_[1]))
						return result::illformed_json;
					return parse_number();
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					return parse_number();
				case '\0':
					return result::incomplete_json;
				default:
					return result::illformed_json;
			}
		}
		
		
		char skip() noexcept {
			constexpr auto t = true;
			constexpr auto f = false;
			static constexpr bool map[] = {
				 f,f,f,f,f,f,f,f,f,t,t,f,f,t,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,t,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,
				 f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f
			};
			static_assert(sizeof(map) == 256, "Invalid whitespace map");
			while(map[unsigned(*cursor_)])
				++cursor_;
			return *cursor_;
		}
		
		
		result parse_object() {
			values_.emplace_back(value_type::object);
			auto const original_size = values_.size();
			++cursor_;
			auto length = size_type(0);
			if(skip() != '}')
				for(;;) {
					if(*cursor_ != '\"')
						return result::illformed_json;
					parse_string();
					if(skip() != ':')
						return result::illformed_json;
					++cursor_;
					auto const e = parse_value();
					if(e != result::ok)
						return e;
					++length;
					auto const delim = skip();
					if(delim == ',') {
						++cursor_;
						skip();
						continue;
					}
					if(delim != '}')
						return result::illformed_json;
					break;
				}
			++cursor_;
			values_[original_size - 1].members_info(length,
			                                        values_.size() - original_size);
			return result::ok;
		}
		
		
		result parse_array() {
			values_.emplace_back(value_type::array);
			auto const original_size = values_.size();
			++cursor_;
			auto length = size_type(0);
			if(skip() != ']')				
				for(;;) {
					auto const e = parse_value();
					if(e != result::ok)
						return e;
					++length;
					auto const delim = skip();
					if(delim == ',') {
						++cursor_;
						continue;
					}
					if(delim != ']')
						return result::illformed_json;
					break;
				}
			++cursor_;
			values_[original_size - 1].members_info(length,
			                                        values_.size() - original_size);
			return result::ok;
		}
		
		
		result parse_string() {
			auto const* mark = ++cursor_;
			for(;;)
				switch(*cursor_) {
					case '\n':
					case '\0':
						return result::unclosed_string;
					case '\\':
						return parse_escaped_string(mark);
					case '"':
						values_.emplace_back(value_type::string,
						                     mark,
											 size_type(cursor_ - mark));
						++cursor_;
						return result::ok;
					default:
						++cursor_;
						continue;
				}
		}
		
		
		result parse_escaped_string(char const* mark) {
			auto* p = cursor_;
			auto escape = parse_escaped_character(p);
			if(escape != result::ok)
				return escape;
			for(;;)
				switch(*cursor_) {
					case '\n':
					case '\0':
						return result::unclosed_string;
					case '\\':
						escape = parse_escaped_character(p);
						if(escape !=  result::ok)
							return escape;
						continue;
					case '"':
						values_.emplace_back(value_type::string,
						                     mark,
											 size_type(p - mark));
						++cursor_;
						return result::ok;
					default:
						*p++ = *cursor_++;
						continue;
				}
		}
		
		
		static std::uint16_t hex_digit(char c) noexcept {
			switch(c) {
				case '0': return 0;
				case '1': return 1;
				case '2': return 2;
				case '3': return 3;
				case '4': return 4;
				case '5': return 5;
				case '6': return 6;
				case '7': return 7;
				case '8': return 8;
				case '9': return 9;
				case 'a': case 'A': return 10;
				case 'b': case 'B': return 11;
				case 'c': case 'C': return 12;
				case 'd': case 'D': return 13;
				case 'e': case 'E': return 14;
				case 'f': case 'F': return 15;
				default: return 16;
			}
		}
		
		
		result parse_code_point(char*& p) noexcept {
			++cursor_;
			auto const q1 = hex_digit(*cursor_);
			if(q1 == 16)
				return result::invalid_escape_sequence;
			++cursor_;
			auto const q2 = hex_digit(*cursor_);
			if(q2 == 16)
				return result::invalid_escape_sequence;
			++cursor_;
			auto const q3 = hex_digit(*cursor_);
			if(q3 == 16)
				return result::invalid_escape_sequence;
			++cursor_;
			auto const q4 = hex_digit(*cursor_);
			if(q4 == 16)
				return result::invalid_escape_sequence;
			++cursor_;
			auto const cp = std::uint16_t((q1 << 12) | (q2 << 8) | (q3 << 4) | q4);
			if(cp <= 0x7F) {
				*p++ = char(cp);
			} else if(cp <= 0x7FF) {
				*p++ = char(0xC0 | (cp >> 6));
				*p++ = 0x80 | (cp & 0x3F);
			} else {
				*p++ = char(0xE0 | (cp >> 12));
				*p++ = char(0x80 | ((cp >> 6) & 0x3F));
				*p++ = char(0x80 | (cp & 0x3F));
			}
			return result::ok;
		}


		result parse_escaped_character(char*& p) noexcept {
			++cursor_;
			switch(*cursor_) {
				case '"':
					*p++ = '"';
					++cursor_;
					return result::ok;
				case '\\':
					*p++ = '\\';
					++cursor_;
					return result::ok;
				case '/':
					*p++ = '/';
					++cursor_;
					return result::ok;
				case 'b':
					*p++ = '\b';
					++cursor_;
					return result::ok;
				case 'f':
					*p++ = '\f';
					++cursor_;
					return result::ok;
				case 'n':
					*p++ = '\n';
					++cursor_;
					return result::ok;
				case 'r':
					*p++ = '\r';
					++cursor_;
					return result::ok;
				case 't':
					*p++ = '\t';
					++cursor_;
					return result::ok;
				case 'u':
					return parse_code_point(p);
				default:
					return result::invalid_escape_sequence;
			}
		}
		
		
		result parse_null() {
			auto const is_null = cursor_[1] == 'u'
				&& cursor_[2] == 'l'
				&& cursor_[3] == 'l';
			if(!is_null)
				return result::illformed_json;
			cursor_ += 4;
			values_.push_back(value::null);
			return result::ok;
		}
		
		
		result parse_true() {
			auto const is_true = cursor_[1] == 'r'
				&& cursor_[2] == 'u'
				&& cursor_[3] == 'e';
			if(!is_true)
				return result::illformed_json;
			cursor_ += 4;
			values_.emplace_back(true);
			return result::ok;
		}
		
		
		result parse_false() {
			auto const is_false = cursor_[1] == 'a'
				&& cursor_[2] == 'l'
				&& cursor_[3] == 's'
				&& cursor_[4] == 'e';
			if(!is_false)
				return result::illformed_json;
			cursor_ += 5;
			values_.emplace_back(false);
			return result::ok;
		}
		
		
		result parse_number() {
			auto const* mark = cursor_++;
			while(is_digit(*cursor_))
				++cursor_;
			if(*cursor_ == '.') {
				++cursor_;
				if(!is_digit(*cursor_)) {
					std::printf("No digit after point: %c\n", *cursor_);
					return result::invalid_number;
				}
				++cursor_;
				while(is_digit(*cursor_))
					++cursor_;
			}
			if(*cursor_ == 'e' || *cursor_ == 'E') {
				++cursor_;
				if(*cursor_ == '+' || *cursor_ == '-') {
					++cursor_;
				}
				if(!is_digit(*cursor_)) {
					std::printf("No digit after exponent\n");
					return result::invalid_number;
				}
				while(is_digit(*cursor_))
					++cursor_;
			}
			auto const length = size_type(cursor_ - mark);
			values_.emplace_back(value_type::number, mark, length);
			return result::ok;
		}
		
	}; // parser
	
	
} // namespace jessy
