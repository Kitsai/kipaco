#pragma once

#include "ParseResult.hpp"
#include <functional>
#include <string_view>

namespace kipaco {

template<typename T, typename Input = std::string_view>
class Parser {
    private:
        std::function<ParseResult<T>(Input)> parse_fn;

    public:

    explicit Parser(std::function<ParseResult<T, Input>(Input)> fn) : parse_fn(std::move(fn)) {}

    ParseResult<T, Input> parse(Input input) {
        return parse_fn(input);
    }

    template<typename U>
    Parser<U, Input> bind(const Parser<U, Input>& other) const {
        return Parser{[first = this->parse_fn, second = other.parse_fn](Input input) {
            ParseResult<T, Input> result1 = first(input);

            if(result1.is_failure()) {
                const auto& fail = result1.get_failure();
                return ParseResult<U, Input>{
                    typename ParseResult<U, Input>::Failure{
                        fail.error, fail.position
                    }
                };
            }

            const auto& success = result1.get_success();
            return second(success.remaining);
        }};
    }

    template<typename F>
    auto map(F&& fn) const {
        using U = std::invoke_result_t<F, const T&>;

        return Parser<U, Input>{[parse_fn = this->parse_fn, fn = std::forward<F>(fn)](Input input) {
            ParseResult<T, Input> result = parse_fn(input);
            if(result.is_failure()) {
                const auto& fail = result.get_failure();
                return ParseResult<U, Input>{
                    typename ParseResult<U, Input>::Failure{fail.error, fail.position}
                };
            }
            const auto& success = result.get_success();
            return ParseResult<U, Input>{
                typename ParseResult<U, Input>::Success{fn(success.value), success.remaining}
            };
        }};
    }

};
}
