#include "basic_parsers/literal.h"
#include "catch2/catch_test_macros.hpp"
#include "kipaco.h"
#include <catch2/catch_all.hpp>
#include <string>
#include <variant>

using namespace kipaco;

TEST_CASE("Testing char parsers", "[basic_parsers, char]") {
    SECTION("any char") {
        std::string input = "teste";

        auto parser = kipaco::char_parser::any_char();

        auto res = parser.parse(input);

        REQUIRE(res.has_value());
        REQUIRE(res->remainder == "este");
        REQUIRE(res->match == 't');

        input = "";

        auto res2 = parser.parse(input);

        REQUIRE(res2.has_error());
        REQUIRE(!res2.has_value());
        REQUIRE(res2.error() == input);
    }

    SECTION("chosen char") {
        std::string input = "teste";

        auto parser = kipaco::char_parser::chosen_char('t');

        auto res1 = parser.parse(input);

        REQUIRE(res1.has_value());
        REQUIRE(res1->remainder == "este");
        REQUIRE(res1->match == 't');

        input = "falha";

        auto res2 = parser.parse(input);

        REQUIRE(res2.has_error());
        REQUIRE(res2.error() == "falha");
    }
}

TEST_CASE("Pure Parser", "[basic_parsers, pure]") {
    auto parser = pure();

    std::string input = "teste";

    auto res = parser.parse(input);

    REQUIRE(res.has_value());
    REQUIRE(res->remainder.empty());
    REQUIRE(res->match == "teste");
}

TEST_CASE("Pure Parser interaction", "[pure, pred]") {
    auto pred = [](const std::string& s) { return s == "teste"; };

    auto parser = pure().pred(pred);

    SECTION("Should accept the string teste") {
        auto input = "teste";

        auto res = parser.parse(input);

        REQUIRE(res.has_value());
        REQUIRE(res->remainder.empty());
        REQUIRE(res->match == "teste");
    }

    SECTION("Should not recognize anything else") {
        auto input = "falha";

        auto res = parser.parse(input);

        REQUIRE(res.has_error());
        REQUIRE(res.error() == "falha");
    }
}

TEST_CASE("Literal parser", "[basic_parsers, literal]") {
    auto parser = literal("teste");

    SECTION("Should succeed when input starts with given string") {
        auto input = "testes";

        auto res = parser.parse(input);

        REQUIRE(res.has_value());
        REQUIRE(res->remainder == "s");
        REQUIRE(res->match == std::monostate());
    }

    SECTION("Should fail when input does not start with fiven string") {
        auto input = "test";

        auto res = parser.parse(input);

        REQUIRE(res.has_error());
        REQUIRE(res.error() == "test");
    }
}
