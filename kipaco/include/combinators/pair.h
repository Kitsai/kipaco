#pragma once

#include "Parser.h"
#include "basic_parsers/literal.h"
#include <utility>
#include <variant>

namespace kipaco {
template <typename A, typename B> Parser<std::pair<A, B>> pair(const Parser<A>& p1, const Parser<B>& p2) {
    return p1.bind([p2](const A& a) { return p2.map([a](const B& b) { return std::make_pair(a, b); }); });
}

template <typename A, typename B> inline Parser<std::pair<A, B>> operator&(const Parser<A>& p1, const Parser<B>& p2) {
    return pair(p1, p2);
}

template <typename T> inline Parser<std::pair<T, std::monostate>> operator&(const Parser<T>& p, const std::string& s) {
    return pair(p, literal(s));
}

template <typename T> inline Parser<std::pair<std::monostate, T>> operator&(const std::string& s, const Parser<T>& p) {
    return pair(literal(s), p);
}
} // namespace kipaco
