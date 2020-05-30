#pragma once

#include "scrabble.h"

#include <variant>
#include <vector>
#include <string>
#include <optional>
#include <string_view>
#include <memory>


namespace scrabble {

namespace gcg {

struct Pragmata
{
    std::string              keyword;
    std::vector<std::string> arguments;
};

struct Play
{
    std::string player;
    Rack        rack;
    Square      square;
    std::string word;
    int         score;
    int         total_score; // cumulative score after turn
};

struct PassedTurn
{
    std::string player;
    std::string rack;
    int         total_score; // cumulative score after turn
};

// used if the tiles exchanged are known
struct TileExchangeKnown
{
    std::string player;
    Rack        tiles_exchanged;
    int         total_score; // cumulative score after turn
};

struct TileExchangeCount
{
    std::string player;
    int         num_tiles;
    int         total_score;
};

struct PhoneyRemoved
{
    std::string player;
    Rack        rack;
    int         score;
    int         total_score; // cumulative score before turn
};

struct ChallengeBonus
{
    std::string player;
    Rack        rack;
    int         bonus;
    int         total_score;
};

struct LastRackPoints
{
    std::string player;
    Rack        tiles;
    int         score;
    int         total_score;
};

struct TimePenalty
{
    std::string player;
    Rack        rack;
    int         penalty;
    int         total_score;
};

using Move = std::variant<
    Pragmata,
    Play,
    PassedTurn,
    TileExchangeKnown,
    TileExchangeCount,
    PhoneyRemoved,
    ChallengeBonus,
    LastRackPoints,
    TimePenalty
>;

struct Parser
{
    Parser();
    ~Parser() noexcept;

    std::optional<Move> parse_line(const std::string& line)
    {
        return parse_line(line.c_str());
    }
    std::optional<Move> parse_line(const char* line);
    // std::optional<Pragmata> parse_pragmata(std::string_view line);
    // std::optional<Play> parse_play(std::string_view line);
    // std::optional<PassedTurn> parse_passed_turn(std::string_view line);
    // std::optional<TileExchangeKnown> parse_tile_exchange_known(
    //         std::string_view line);
    // std::optional<TileExchangeCount> parse_tile_exchange_count(
    //         std::string_view line);
    // std::optional<PhoneyRemoved> parse_phoney_removed(std::string_view line);
    // std::optional<ChallengeBonus> parse_challenge_bonus(std::string_view line);
    // std::optional<LastRackPoints> parse_last_rack_points(std::string_view line);
    // std::optional<TimePenalty> parse_time_penalty(std::string_view line);

private:
    struct Data;
    std::unique_ptr<Data> data;
};

} // ~gcg

} // ~scrabble
