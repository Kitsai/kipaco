#pragma once

#include "Parser.h"
#include "combinators/pair.h"
#include <variant>

namespace kipaco {
template <typename T, typename U> Parser<T> left(const Parser<T>& lhs, const Parser<U>& rhs) {
    return pair(lhs, rhs).map([](std::pair<T, U> res) { return std::move(res.first); });
}

template <typename T, typename U> inline Parser<T> operator<<(const Parser<T>& lhs, const Parser<U>& rhs) {
    return left(lhs, rhs);
}

template <typename T> inline Parser<T> operator<<(const Parser<T>& lhs, const std::string& rhs) {
    return left(lhs, literal(rhs));
}

template <typename T> inline Parser<std::monostate> operator<<(const std::string& lhs, const Parser<T>& rhs) {
    return left(literal(lhs), rhs);
}

template <typename T, typename U> Parser<U> right(const Parser<T>& lhs, const Parser<U>& rhs) {
    return pair(lhs, rhs).map([](std::pair<T, U> res) { return std::move(res.second); });
}

template <typename T, typename U> inline Parser<U> operator>>(const Parser<T>& lhs, const Parser<U>& rhs) {
    return right(lhs, rhs);
}

template <typename T> inline Parser<std::monostate> operator>>(const Parser<T>& lhs, const std::string& rhs) {
    return right(lhs, literal(rhs));
}

template <typename T> inline Parser<T> operator>>(const std::string& lhs, const Parser<T>& rhs) {
    return right(literal(lhs), rhs);
}
} // namespace kipaco
