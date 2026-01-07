#pragma once

#include "Parser.hpp"

#include <string_view>
namespace kipaco {
    template<typename T, typename Input = std::string_view>
    Parser<T, Input> pure(T value) {
        return Parser<T, Input>{[value = std::move(value)](Input input) {
            return ParseResult<T, Input>{
                typename ParseResult<T, Input>::Success{value, input}
            };
        }};
    }
}
