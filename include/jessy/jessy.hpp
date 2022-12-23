// This file is part of jessy library
// Copyright 2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <cstdint>
#include <optional>
#include <string_view>

#include <yyjson.h>


namespace jessy {
    
    namespace detail {
        
        inline yyjson_val null_value = yyjson_val {
            .tag = YYJSON_TYPE_NULL,
            .uni = yyjson_val_uni{}
        };
        
    } // namespace detail
    
    
    enum class value_type {
        null = YYJSON_TYPE_NULL,
        boolean = YYJSON_TYPE_BOOL,
        uinteger = (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT),
        integer = (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT),
        floating = (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL),
        string = YYJSON_TYPE_STR,
        array = YYJSON_TYPE_ARR,
        object = YYJSON_TYPE_OBJ
    }; // value_type
    
    
    inline std::string_view constexpr entitle(value_type type) noexcept {
        switch(type) {
            case value_type::null:
                return {"null"};
            case value_type::boolean:
                return {"boolean"};
            case value_type::integer:
                return {"integer"};
            case value_type::uinteger:
                return {"unsigned"};
            case value_type::floating:
                return {"double"};
            case value_type::string:
                return {"string"};
            case value_type::array:
                return {"array"};
            case value_type::object:
                return {"object"};
            default:
                return {"unknown"};
        }
    }
    
    
    class document;
    class value {
    friend class document;
        yyjson_val* val_;
        
        value(yyjson_val* val) noexcept: val_{val} { }
        
    public:
    
        value() = delete;
        value(value const&) = default;
        value& operator = (value const&) = default;
        
        
        template<std::size_t N>
        value operator [](char const (&key)[N]) const noexcept {
            auto* val = yyjson_obj_getn(val_, key, N - 1);
            if(!val)
                return value{&detail::null_value};
            return value{val};
        }
        
        
        bool is_null() const noexcept {
            return unsafe_yyjson_is_null(val_);
        }
        
        bool is_object() const noexcept {
            return unsafe_yyjson_is_obj(val_);
        }
        
        bool is_array() const noexcept {
            return unsafe_yyjson_is_arr(val_);
        }
        
        
        value_type type() const noexcept {
            return (value_type)(val_->tag & (YYJSON_TYPE_MASK | YYJSON_SUBTYPE_MASK));
        }
        
        
        std::optional<bool> as_bool() const noexcept {
            if(!unsafe_yyjson_is_bool(val_))
                return std::nullopt;
            return {unsafe_yyjson_get_bool(val_)};
        }


        std::optional<std::int64_t> as_int() const noexcept {
            switch(val_->tag & (YYJSON_TYPE_MASK | YYJSON_SUBTYPE_MASK)) {
                case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT:
                    return static_cast<std::int64_t>(unsafe_yyjson_get_uint(val_));
                case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT:
                    return unsafe_yyjson_get_sint(val_);
                default:
                    return std::nullopt;
            }
        }


        std::optional<std::uint64_t> as_uint() const noexcept {
            if(!unsafe_yyjson_is_uint(val_))
                return std::nullopt;
            return {unsafe_yyjson_get_uint(val_)};
        }


        std::optional<double> as_double() const noexcept {
            switch(val_->tag & (YYJSON_TYPE_MASK | YYJSON_SUBTYPE_MASK)) {
                case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT:
                    return static_cast<double>(unsafe_yyjson_get_uint(val_));
                case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT:
                    return static_cast<double>(unsafe_yyjson_get_sint(val_));
                case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL:
                    return unsafe_yyjson_get_real(val_);
                default:
                    return std::nullopt;
            }
        }


        std::optional<std::string_view> as_string_view() const noexcept {
            if(!unsafe_yyjson_is_str(val_))
                return std::nullopt;
            return {std::string_view{unsafe_yyjson_get_str(val_),
                                     unsafe_yyjson_get_len(val_)}};
        }
        
    }; // value
    
    
    class document {
        yyjson_doc* doc_;
        
        document(yyjson_doc* doc) noexcept: doc_{doc}, root{doc->root} { }
        
    public:
    
        value root;
    
        static std::optional<document> read(std::string_view view) noexcept {
            auto* doc = yyjson_read(view.data(), view.size(), 0);
            if(!doc)
                return std::nullopt;
            return document{doc};
        }

    
        document() = delete;
        document(document const&) = delete;
        document& operator = (document const&) = delete;
        
        ~document() {
            yyjson_doc_free(doc_);
        }
        
        
        document(document&& other) noexcept: doc_{other.doc_}, root{other.root} {
            other.doc_ = nullptr;
        }
        
        
        document& operator = (document&& other) noexcept {
            yyjson_doc_free(doc_);
            doc_ = other.doc_;
            other.doc_ = nullptr;
            root = other.root;
            return *this;
        }
            
    }; // document
    
    
} // namespace jessy
