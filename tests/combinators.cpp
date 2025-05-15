#include <catch2/catch_all.hpp>
#include <string>
#include <variant>

#include "basic_parsers/char.h"
#include "catch2/catch_test_macros.hpp"
#include "kipaco.h"

using namespace kipaco;

TEST_CASE("Predicate Combinator") {
    auto is_digit = [](const char& c) { return std::isdigit(c); };
    auto is_vowel = [](const char& c) { return std::string("aeiouAEIOU").find(c) != std::string::npos; };

    SECTION("Successful predicate") {
        auto parser = char_parser::any_char().pred(is_digit);
        auto result = parser.parse("5apples");

        REQUIRE(result.has_value());
        REQUIRE(result->match == '5');
        REQUIRE(result->remainder == "apples");
    }

    SECTION("Failed predicate preserves original input") {
        auto parser = char_parser::any_char().pred(is_vowel);
        auto result = parser.parse("zebra");

        REQUIRE(result.has_error());
        REQUIRE(result.error() == "zebra"); // Original input
    }

    SECTION("Propagates underlying parser failure") {
        auto parser = char_parser::any_char().pred(is_digit);
        auto result = parser.parse(""); // Empty input

        REQUIRE(result.has_error());
        REQUIRE(result.error().empty());
    }
}

TEST_CASE("Bind Combinator") {
    SECTION("Successful chained parsing") {
        // Parse first char, then require next char to match
        auto parser = char_parser::any_char().bind([](char c) { return char_parser::chosen_char(c); });

        auto result = parser.parse("aabc");
        REQUIRE(result.has_value());
        REQUIRE(result->match == 'a');
        REQUIRE(result->remainder == "bc");
    }

    SECTION("Propagates first parser failure") {
        auto parser = char_parser::chosen_char('@').bind([](char) { return char_parser::any_char(); }); // Never reached

        auto result = parser.parse("missing");
        REQUIRE(result.has_error());
        REQUIRE(result.error() == "missing");
    }

    SECTION("Complex bind chain") {
        // Parse "a" followed by "b" followed by "c"
        auto parser =
            char_parser::chosen_char('a').bind([](auto) { return char_parser::chosen_char('b'); }).bind([](auto) {
                return char_parser::chosen_char('c');
            });

        auto success = parser.parse("abc");
        REQUIRE(success.has_value());
        REQUIRE(success->remainder.empty());

        auto failure = parser.parse("abd");
        REQUIRE(failure.has_error());
        REQUIRE(failure.error() == "d"); // Failure at final parser
    }

    SECTION("Operator") {
        // Parse "a" followed by "b" followed by "c"
        auto parser = char_parser::chosen_char('a') * char_parser::chosen_char('b') * char_parser::chosen_char('c');

        auto success = parser.parse("abc");
        REQUIRE(success.has_value());
        REQUIRE(success->remainder.empty());

        auto failure = parser.parse("abd");
        REQUIRE(failure.has_error());
        REQUIRE(failure.error() == "d"); // Failure at final parser
    }
}

TEST_CASE("Map Combinator", "[combinators, map]") {
    auto pred = [](char c) { return std::stoi(std::string(1, c)); };

    auto parser = char_parser::any_char().map(pred);

    SECTION("Should map a digit to int") {
        auto input = "12";

        auto res = parser.parse(input);

        REQUIRE(res.has_value());
        REQUIRE(res->remainder == "2");
        REQUIRE(res->match == 1);
    }

    SECTION("Should fail when not digit") {
        auto input = "A2";

        REQUIRE_THROWS(parser.parse(input));
    }
}

TEST_CASE("Negate Combinator", "[combinators, negate]") {
    auto parser = char_parser::chosen_char('T').negate();

    SECTION("Should fail when the parser succeds") {
        auto input = "Teste";

        auto res = parser.parse(input);

        REQUIRE(res.has_error());
        REQUIRE(res.error() == "Teste");
    }

    SECTION("Should Succed when parser fails") {
        auto input = "teste";

        auto res = parser.parse(input);

        REQUIRE(res.has_value());
        REQUIRE(res->remainder == "teste");
        REQUIRE(res->match == std::monostate());
    }
}

TEST_CASE("pair combinator", "[parser][combinator][pair]") {
    SECTION("literal + literal succeeds") {
        auto parser = literal("foo") & literal("bar");

        auto result = parser.parse("foobar");

        REQUIRE(result.has_value());
        REQUIRE(result->match.first == std::monostate());  // monostate
        REQUIRE(result->match.second == std::monostate()); // monostate
        REQUIRE(result->remainder == "");
    }

    SECTION("literal + any_char succeeds") {
        auto parser = literal("a") & char_parser::any_char();

        auto result = parser.parse("ab");

        REQUIRE(result.has_value());
        REQUIRE(result->match.first == std::monostate()); // monostate
        REQUIRE(result->match.second == 'b');
        REQUIRE(result->remainder == "");
    }

    SECTION("any_char + literal succeeds") {
        auto parser = char_parser::any_char() & literal("b");

        auto result = parser.parse("ab");

        REQUIRE(result.has_value());
        REQUIRE(result->match.first == 'a');
        REQUIRE(result->match.second == std::monostate()); // monostate
        REQUIRE(result->remainder == "");
    }

    SECTION("Fails if first parser fails") {
        auto parser = literal("foo") & literal("bar");

        auto result = parser.parse("bazbar");

        REQUIRE(result.has_error());
    }

    SECTION("Fails if second parser fails") {
        auto parser = literal("foo") & literal("bar");

        auto result = parser.parse("foobaz");

        REQUIRE(result.has_error());
    }

    SECTION("Operator+ with monostate and char") {
        auto parser = "a" & char_parser::any_char(); // "a" becomes literal("a")

        auto result = parser.parse("ab");

        REQUIRE(result.has_value());
        REQUIRE(result->match.first == std::monostate()); // monostate
        REQUIRE(result->match.second == 'b');
        REQUIRE(result->remainder == "");
    }

    SECTION("Nested pair") {
        auto parser = char_parser::any_char() & (literal("x") & char_parser::any_char());

        auto result = parser.parse("axy");

        REQUIRE(result.has_value());

        auto outer = result->match;
        auto c1 = outer.first;
        auto inner = outer.second;

        REQUIRE(c1 == 'a');
        REQUIRE(inner.first == std::monostate()); // monostate
        REQUIRE(inner.second == 'y');
        REQUIRE(result->remainder == "");
    }
}

TEST_CASE("Left and Right Combinators") {
    using namespace kipaco::char_parser;
    SECTION("Left keeps left value, discards right") {
        auto parser = any_char() << literal("x");
        auto result = parser.parse("ax");

        REQUIRE(result.has_value());
        REQUIRE(result->match == 'a');
        REQUIRE(result->remainder == "");
    }

    SECTION("Right keeps right value, discards left") {
        auto parser = literal("a") >> any_char();
        auto result = parser.parse("ab");

        REQUIRE(result.has_value());
        REQUIRE(result->match == 'b');
        REQUIRE(result->remainder == "");
    }

    SECTION("Left fails if right parser fails") {
        auto parser = any_char() << literal("x");
        auto result = parser.parse("ay"); // 'y' ≠ 'x'

        REQUIRE(result.has_error());
        REQUIRE(result.error() == "y");
    }

    SECTION("Right fails if left parser fails") {
        auto parser = literal("a") >> any_char();
        auto result = parser.parse("xb"); // 'x' ≠ 'a'

        REQUIRE(result.has_error());
        REQUIRE(result.error() == "xb");
    }

    SECTION("Left with string literal on left") {
        auto parser = std::string("foo") << literal("bar");
        auto result = parser.parse("foobar");

        REQUIRE(result.has_value());
        REQUIRE(result->match == std::monostate{});
        REQUIRE(result->remainder == "");
    }

    SECTION("Right with string literal on right") {
        auto parser = literal("foo") >> std::string("bar");
        auto result = parser.parse("foobar");

        REQUIRE(result.has_value());
        REQUIRE(result->match == std::monostate{});
        REQUIRE(result->remainder == "");
    }

    SECTION("Left fails if any part fails") {
        auto parser = std::string("foo") << literal("bar");
        auto result = parser.parse("foobaz");

        REQUIRE(result.has_error());
        REQUIRE(result.error() == "baz");
    }

    SECTION("Right fails if any part fails") {
        auto parser = std::string("foo") >> literal("bar");
        auto result = parser.parse("f00bar");

        REQUIRE(result.has_error());
        REQUIRE(result.error() == "f00bar");
    }
}

TEST_CASE("many operation", "[multiple][many]") {
    auto parser = char_parser::chosen_char('a').many();

    SECTION("should collect all matches.") {
        auto input = "aaabc";

        auto res = parser(input);

        REQUIRE(res.has_value());
        REQUIRE(res->match.size() == 3);
        REQUIRE(res->match == std::vector<char>{'a', 'a', 'a'});
        REQUIRE(res->remainder == "bc");
    }

    SECTION("should return empty vector on no match") {
        auto input = "bbaa";

        auto res = parser(input);

        REQUIRE(res.has_value());
        REQUIRE(res->match.empty());
        REQUIRE(res->remainder == input);
    }
}

TEST_CASE("many1 operation", "[multiple][many1]") {
    auto parser = char_parser::chosen_char('a').many1();

    SECTION("should collect single match.") {
        auto input = "abc";

        auto res = parser(input);

        REQUIRE(res.has_value());
        REQUIRE(res->match.size() == 1);
        REQUIRE(res->match == std::vector<char>{'a'});
        REQUIRE(res->remainder == "bc");
    }

    SECTION("should collect all matches.") {
        auto input = "aaabc";

        auto res = parser(input);

        REQUIRE(res.has_value());
        REQUIRE(res->match.size() == 3);
        REQUIRE(res->match == std::vector<char>{'a', 'a', 'a'});
        REQUIRE(res->remainder == "bc");
    }

    SECTION("should return error on no match") {
        auto input = "bbaa";

        auto res = parser(input);

        REQUIRE(res.has_error());
        REQUIRE(res.error() == input);
    }
}

TEST_CASE("optional", "[optional]") {
    auto parser = literal("teste").optional();

    SECTION("Should have a value on match") {
        auto input = "teste1";

        auto res = parser(input);

        REQUIRE(res.has_value());
        REQUIRE(res->remainder == "1");
        REQUIRE(res->match.has_value());
        REQUIRE(res->match.value() == std::monostate());
    }

    SECTION("Should not have value on fail") {
        auto input = "Teste1";

        auto res = parser(input);

        REQUIRE(res.has_value());
        REQUIRE(res->remainder == input);
        REQUIRE(!res->match.has_value());
    }
}

TEST_CASE("Either Combinator") {
    SECTION("Parses using first parser if it succeeds") {
        auto parser = literal("yes") | literal("no");
        auto result = parser.parse("yes please");

        REQUIRE(result.has_value());
        REQUIRE(result->match == std::monostate{}); // literal returns monostate
        REQUIRE(result->remainder == " please");
    }

    SECTION("Parses using second parser if first fails") {
        auto parser = literal("yes") | literal("no");
        auto result = parser.parse("no thanks");

        REQUIRE(result.has_value());
        REQUIRE(result->match == std::monostate{});
        REQUIRE(result->remainder == " thanks");
    }

    SECTION("Fails if both parsers fail") {
        auto parser = literal("yes") | literal("no");
        auto result = parser.parse("maybe");

        REQUIRE(result.has_error());
        REQUIRE(result.error() == "maybe");
    }

    SECTION("String overload left | Parser") {
        auto parser = std::string("hi") | literal("hello");
        auto result = parser.parse("hi there");

        REQUIRE(result.has_value());
        REQUIRE(result->remainder == " there");
    }

    SECTION("Parser | String overload") {
        auto parser = literal("hello") | std::string("hi");
        auto result = parser.parse("hi again");

        REQUIRE(result.has_value());
        REQUIRE(result->remainder == " again");
    }

    SECTION("Combining multiple options") {
        auto parser = literal("red") | literal("green") | literal("blue");
        auto result = parser.parse("greenlight");

        REQUIRE(result.has_value());
        REQUIRE(result->remainder == "light");
    }

    SECTION("First match wins") {
        // If both could match, only the first is chosen
        auto parser = literal("go") | literal("gopher");
        auto result = parser.parse("goose");

        REQUIRE(result.has_value());
        REQUIRE(result->remainder == "ose");
    }
}

TEST_CASE("many(n) combinator") {
    auto parser = char_parser::digit().many(2);

    SECTION("Parses at least n digits and continues") {
        auto result = parser.parse("123abc");

        REQUIRE(result.has_value());
        REQUIRE(result->match == std::vector<char>{'1', '2', '3'});
        REQUIRE(result->remainder == "abc");
    }

    SECTION("Fails if fewer than n digits") {
        auto result = parser.parse("9abc");

        REQUIRE(result.has_error());
        REQUIRE(result.error() == "9abc");
    }

    SECTION("Parses exactly n digits when no more available") {
        auto result = parser.parse("56x");

        REQUIRE(result.has_value());
        REQUIRE(result->match == std::vector<char>{'5', '6'});
        REQUIRE(result->remainder == "x");
    }
}
