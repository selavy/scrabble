#include "gcg.h"
#include <re2/re2.h>
#include <sstream>

namespace hume {

namespace gcg {

struct Parser::Data
{
    // const re2::RE2 pragmata1_regex
    //     = R"(#(\w+)\s*)";
    // const re2::RE2 pragmata2_regex =
    //     = R"(#(\w+)\s+(.*))";
    const re2::RE2 play_regex
        = R"(>(\w+):\s+([A-Z\?]+)\s+(\w+)\s+([A-Za-z\.]+)\s+([+-]?\d+)\s+([+-]?\d+)\s*)";
    const re2::RE2 passed_turn_regex
        = R"(>(\w+):\s+([A-Z\?]+)\s+-\s+\+0\s+(\d+)\s*)";
    const re2::RE2 tile_exchange_known_regex
        = R"(>(\w+):\s+([A-Z\?]+)\s+-([A-Z\?]+)\s+\+0\s+(\d+)\s*)";
    const re2::RE2 tile_exchange_count_regex =
        R"(>(\w+):\s+([A-Z\?]+)\s+-([1-7])\s+\+0\s+(\d+)\s*)";
    const re2::RE2 phoney_removed_regex =
        R"(>(\w+):\s+([A-Z\?]+)\s+--\s+(-\d+)\s+(\d+)\s*)";
    const re2::RE2 challenge_bonus_regex =
        R"(>(\w+):\s+([A-Z\?]+)\s+\(challenge\)\s+([+-]?\d+)\s+([+-]?\d+)\s*)";
    const re2::RE2 last_rack_points_regex =
        R"(>(\w+):\s+\(([A-Z\?]+)\)\s+([+-]?\d+)\s+([+-]?\d+)\s*)";
    const re2::RE2 time_penalty_regex =
        R"(>(\w+):\s+([A-Z\?]+)\s+\(time\)\s+([+-]?\d+)\s+([+-]?\d+)\s*)";
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
    if (auto result = parse_tile_exchange_count(line)) {
        return *result;
    }
    if (auto result = parse_phoney_removed(line)) {
        return *result;
    }
    if (auto result = parse_challenge_bonus(line)) {
        return *result;
    }
    if (auto result = parse_last_rack_points(line)) {
        return *result;
    }
    if (auto result = parse_time_penalty(line)) {
        return *result;
    }
    return std::nullopt;
}

std::optional<Pragmata> Parser::parse_pragmata(std::string_view line)
{
    Pragmata result;
    if (line.empty() || line[0] != '#') {
        return std::nullopt;
    }
    // auto& regex = data->pragmata_regex;
    // assert(regex.ok());
    // std::string rest;
    // if (!re2::RE2::FullMatch(line, regex, &result.keyword, &rest)) {
    //     return std::nullopt;
    // }
    // TODO: better string split. maybe ranges?
    std::string rest{line.data(), line.size()};
    std::stringstream ss{rest};
    std::string token;
    ss >> result.keyword;
    while (ss >> token) {
        result.arguments.emplace_back(std::move(token));
    }
    if (result.keyword.empty()) {
        return std::nullopt;
    }
    result.keyword.erase(0, 1);
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
    result.direction = gcg_direction(sqname);
    // for (auto& c : result.word) {
    //     if (c == '?') {
    //         continue;
    //     } else if ('A' <= c && c <= 'Z') {
    //         c = static_cast<char>((c - 'A') + 'a');
    //     } else if ('a' <= c && c <= 'z') {
    //         c = static_cast<char>((c - 'a') + 'A');
    //     } else {
    //         return std::nullopt;
    //     }
    // }
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

std::optional<TileExchangeCount> Parser::parse_tile_exchange_count(
        std::string_view line)
{
    TileExchangeCount result;
    auto& regex = data->tile_exchange_count_regex;
    assert(regex.ok());
    std::string rack;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack,
                &result.num_tiles, &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    return result;
}

std::optional<PhoneyRemoved> Parser::parse_phoney_removed(
        std::string_view line)
{
    PhoneyRemoved result;
    auto& regex = data->phoney_removed_regex;
    assert(regex.ok());
    std::string rack;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack,
                &result.score, &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    return result;
}

std::optional<ChallengeBonus> Parser::parse_challenge_bonus(
        std::string_view line)
{
    ChallengeBonus result;
    auto& regex = data->challenge_bonus_regex;
    assert(regex.ok());
    std::string rack;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack,
                &result.bonus, &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    return result;
}

std::optional<LastRackPoints> Parser::parse_last_rack_points(
        std::string_view line)
{
    LastRackPoints result;
    auto& regex = data->last_rack_points_regex;
    assert(regex.ok());
    std::string rack;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack,
                &result.score, &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    return result;
}

std::optional<TimePenalty> Parser::parse_time_penalty(std::string_view line)
{
    TimePenalty result;
    auto& regex = data->time_penalty_regex;
    assert(regex.ok());
    std::string rack;
    if (!re2::RE2::FullMatch(line, regex, &result.player, &rack,
                &result.penalty, &result.total_score)) {
        return std::nullopt;
    }
    result.rack = rack;
    return result;
}

} // ~gcg

} // ~hume
