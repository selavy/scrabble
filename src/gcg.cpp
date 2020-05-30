#include "gcg.h"
#include <re2/re2.h>
#include <sstream>

namespace scrabble {

namespace gcg {

struct Parser::Data
{
    re2::RE2 pragmata_regex = R"(#(\w+) (.*))";
    // re2::RE2 play_regex;
    // re2::RE2 passed_turn_regex;
    // re2::RE2 tile_exchange_known_regex;
    // re2::RE2 tile_exchange_count_regex;
    // re2::RE2 phoney_removed_regex;
    // re2::RE2 challenge_bonus_regex;
    // re2::RE2 last_rack_points_regex;
    // re2::RE2 time_penalty_regex;
};

Parser::Parser() : data(std::make_unique<Data>()) {}
Parser::~Parser() noexcept {}

std::optional<Move> Parser::parse_line(const char* line)
{
    Pragmata res;
    auto& regex = data->pragmata_regex;
    assert(regex.ok());
    std::string rest;
    if (!re2::RE2::FullMatch(line, regex, &res.keyword, &rest)) {
        return std::nullopt;
    }
    // TODO: better string split. maybe ranges?
    std::stringstream ss{rest};
    std::string token;
    while (ss >> token) {
        res.arguments.emplace_back(std::move(token));
    }
    return res;
}

} // ~gcg

} // ~scrabble
