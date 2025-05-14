#pragma once

#include "Parser.h"

namespace kipaco {
template <typename T> Parser<T> pred(Parser<T> p, std::function<bool(T)> f) { return p.pred(f); }
} // namespace kipaco
