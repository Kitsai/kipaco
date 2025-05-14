#pragma once

#include "result.hpp"

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

namespace kipaco {

template <typename T> class Parser {
  public:
    using element_type = T;

    struct ParseResultType {
        std::string remainder;
        T match;
    };

    using ParseResult = cpp::result<ParseResultType, std::string>;

    using ParserType = std::function<ParseResult(const std::string& input)>;

  private:
    std::shared_ptr<ParserType> method;

  public:
    Parser(ParserType func) : method(std::make_shared<ParserType>(std::move(func))) {}

    [[nodiscard]] ParseResult parse(const std::string& input) const { return (*method)(input); }

    inline ParseResult operator()(const std::string& input) const { return parse(input); }

    template <typename F> auto bind(F f) const {
        auto method = this->method;

        using ReturnType = decltype(f(std::declval<T>()));
        using NewT = typename ReturnType::element_type;

        return Parser<NewT>([method, f](const std::string& input) -> typename Parser<NewT>::ParseResult {
            auto ret = (*method)(input);
            if(ret.has_error())
                return cpp::fail(ret.error());

            const auto& rem = ret->remainder;
            const auto& match = ret->match;

            return f(match).parse(rem);
        });
    }

    Parser<T> pred(std::function<bool(const T&)> predicate) const {
        auto method = this->method;
        return {[method, predicate](const std::string& input) -> ParseResult {
            auto res = (*method)(input);

            if(res.has_value() && predicate(res->match))
                return res;

            return cpp::fail(input);
        }};
    }

    [[nodiscard]] Parser<std::monostate> negate() const {
        auto method = this->method;

        return {[method](const std::string& input) -> Parser<std::monostate>::ParseResult {
            auto res = (*method)(input);

            if(res.has_error())
                return Parser<std::monostate>::ParseResultType{input, {}};

            return cpp::fail(input);
        }};
    }

    template <typename F> auto map(F f) const {
        auto method = this->method;

        using ReturnType = decltype(f(std::declval<T>()));
        using NewT = std::decay_t<ReturnType>;

        return Parser<NewT>([method, f](const std::string& input) -> typename Parser<NewT>::ParseResult {
            auto res = (*method)(input);

            if(res.has_error())
                return cpp::fail(res.error());

            return typename Parser<NewT>::ParseResultType{res->remainder, f(res->match)};
        });
    }

    Parser<std::vector<T>> many() {
        auto method = this->method;
        return {[method](const std::string& input) -> typename Parser<std::vector<T>>::ParseResult {
            std::vector<T> vec{};

            std::string new_input = input;

            while(true) {
                auto res = (*method)(new_input);

                if(res.has_error())
                    break;

                vec.push_back(res->match);
                new_input = res->remainder;
            }

            return typename Parser<std::vector<T>>::ParseResultType{new_input, vec};
        }};
    }

    Parser<std::vector<T>> many1() {
        auto method = this->method;
        return {[method](const std::string& input) -> typename Parser<std::vector<T>>::ParseResult {
            std::vector<T> vec{};

            auto res = (*method)(input);

            if(res.has_error())
                return cpp::fail(input);

            vec.push_back(res->match);
            auto new_input = res->remainder;

            while(true) {
                auto res = (*method)(new_input);

                if(res.has_error())
                    break;

                vec.push_back(res->match);
                new_input = res->remainder;
            }

            return typename Parser<std::vector<T>>::ParseResultType{new_input, vec};
        }};
    }
};

template <typename T> inline Parser<std::string> operator!(const Parser<T>& p) { return p.negate(); }

} // namespace kipaco
