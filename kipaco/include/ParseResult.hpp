#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace kipaco {
    template<typename T, typename Input = std::string_view>
    struct ParseResult {
        struct Success {
            T value;
            Input remaining;
        };

        struct Failure {
            std::string error;
            Input position;
        };

        std::variant<Success, Failure> result;

        [[nodiscard]] bool is_success() const {
            return std::holds_alternative<Success>(result);
        }

        [[nodiscard]] bool is_failure() const {
            return std::holds_alternative<Failure>(result);
        }

        [[nodiscard]] const Success& get_success() const {
            return std::get<Success>(result);
        }

        [[nodiscard]] const Failure& get_failure() const {
            return std::get<Failure>(result);
        }

        operator bool() const {
            return std::holds_alternative<Success>(result);
        }

    };

    template<typename R>
    concept ParserReturn = requires(R r) {
        { r.is_success() } -> std::convertible_to<bool>;
        { r.is_failure() } -> std::convertible_to<bool>;

        typename R::Success;
        typename R::Failure;

        { r.get_success() } -> std::same_as<const typename R::Success&>;
        { r.get_failure() } -> std::same_as<const typename R::Failure&>;
    };

    static_assert(ParserReturn<ParseResult<int>>);
}
