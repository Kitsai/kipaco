#pragma once

#include "Parser.h"
#include <functional>

namespace kipaco {

template <typename A, typename B> Parser<B> bind(Parser<A> p, std::function<Parser<B>(A)> f) { return p.bind(f); }

template <typename A, typename B> Parser<B> bind(Parser<A> first, Parser<B> second) {
    return first.bind([second](A) -> Parser<B> { return second; });
}

template <typename A, typename B> inline Parser<B> operator*(Parser<A> lhs, Parser<B> rhs) { return bind(lhs, rhs); }

template <typename A, typename B> inline Parser<B> operator*(Parser<A> lhs, std::function<Parser<B>(A)> rhs) {
    return lhs.bind(rhs);
}

} // namespace kipaco
