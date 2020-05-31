#include "gcg.h"
#include <re2/re2.h>
#include <sstream>

namespace hume {

namespace gcg {

struct Parser::Data
{
    const re2::RE2 pragmata_regex
        = R"(#(\w+) (.*))";
    const re2::RE2 play_regex
        = R"(>(\w+):\s+([A-Z\?]+)\s+(\w+)\s+([A-Za-z\.]+)\s+([+-]?\d+)\s+([+-]?\d+)\s*)";
    const re2::RE2 passed_turn_regex
        = R"(>(\w+):\s+([A-Z\?]+)\s+-\s+\+0\s+(\d+)\s*)";
    const re2::RE2 tile_exchange_known_regex
        = R"(>(\w+):\s+([A-Z\?]+)\s+-([A-Z\?]+)\s+\+0\s+(\d+)\s*)";
    // re2::RE2 tile_exchange_count_regex;
    // re2::RE2 phoney_removed_regex;
    // re2::RE2 challenge_bonus_regex;
    // re2::RE2 last_rack_points_regex;
    // re2::RE2 time_penalty_regex;
};

Parser::Parser() : data(std::make_unique<Data>()) {}
Parser::~Parser() noexcept {}

std::optional<Move> Parser::parse_line(std::string_view line)
{
    if (auto result = parse_pragmata(line)) {
        return *result;
    }
    if (auto result = parse_play(line)) {
        return *result;
    }
    if (auto result = parse_passed_turn(line)) {
        return *result;
    }
    if (auto result = parse_tile_exchange_known(line)) {
        return *result;
    }
    return std::nullopt;
}

std::optional<Pragmata> Parser::parse_pragmata(std::string_view line)
{
    Pragmata result;
    auto& regex = data->pragmata_regex;
    assert(regex.ok());
    std::string rest;
    if (!re2::RE2::FullMatch(line, regex, &result.keyword, &rest)) {
        return std::nullopt;
    }
    // TODO: better string split. maybe ranges?
    std::stringstream ss{rest};
    std::string token;
    while (ss >> token) {
        result.arguments.emplace_back(std::move(token));
    }
    return result;
}

std::optional<Play> Parser::parse_play(std::string_view line)
{
    Play result;
    auto& regex = data->play_regex;
    assert(regex.ok());
    std::string rack;
    std::string sqname;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack, &sqname,
                &result.word, &result.score, &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    auto square = Square::from_gcg_name(sqname);
    if (!square) {
        return std::nullopt;
    }

    for (auto& c : result.word) {
        if (c == '?') {
            continue;
        } else if ('A' <= c && c <= 'Z') {
            c = static_cast<char>((c - 'A') + 'a');
        } else if ('a' <= c && c <= 'z') {
            c = static_cast<char>((c - 'a') + 'A');
        } else {
            return std::nullopt;
        }
    }

    result.square = *square;
    return result;
}

std::optional<PassedTurn> Parser::parse_passed_turn(std::string_view line)
{
    PassedTurn result;
    auto& regex = data->passed_turn_regex;
    assert(regex.ok());
    std::string rack;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack,
                &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    return result;
}

std::optional<TileExchangeKnown> Parser::parse_tile_exchange_known(
        std::string_view line)
{
    TileExchangeKnown result;
    auto& regex = data->tile_exchange_known_regex;
    assert(regex.ok());
    std::string rack;
    std::string exchanged;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack, &exchanged,
                &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    result.tiles_exchanged = exchanged;
    return result;
}

} // ~gcg

} // ~hume