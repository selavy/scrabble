#include <catch2/catch.hpp>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cicero/cicero.h>
#include <scrabble.h>
#include <mafsa.h>
#include "test_data.h"

struct Move
{
    Move(std::string w, int s, int d) noexcept
        : word{std::move(w)}, square{s}, direction{d} {}

    Move(std::string iscsq, std::string word) noexcept
        : word{std::move(word)}
        , square{SqNumDir(iscsq.c_str()).sq}
        , direction{SqNumDir(iscsq.c_str()).dir}
    {}

    std::string word;
    int         square;
    int         direction;
};


bool operator<(const Move& lhs, const Move& rhs) noexcept
{
    int c = lhs.word.compare(rhs.word);
    if (c == 0) {
        if (lhs.square == rhs.square) {
            return lhs.direction < rhs.direction;
        } else {
            return lhs.square < rhs.square;
        }
    }
    return c < 0;
}

bool operator==(const Move& lhs, const Move& rhs) noexcept
{
    return lhs.word == rhs.word && lhs.square == rhs.square && lhs.direction == rhs.direction;
}

bool operator!=(const Move& lhs, const Move& rhs) noexcept
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Move& m) noexcept
{
   os << IscSQ(m.direction, m.square) << " " << m.word;
   return os;
}

bool operator==(const std::vector<Move>& lhs, const std::vector<Move>& rhs) noexcept
{
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const std::vector<Move>& lhs, const std::vector<Move>& rhs) noexcept
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const std::vector<Move>& moves) noexcept
{
    os << "[ ";
    for (const auto& move : moves) {
        os << "\"" << move << "\" ";
    }
    os << "]";
    return os;
}

struct Callbacks
{
    Callbacks(Mafsa&& m) noexcept : mafsa_{std::move(m)}, legal_moves_{} {}

    static cicero_edges prefix_edges(void* data, const char* prefix) noexcept
    {
        auto* self = reinterpret_cast<const Callbacks*>(data);
        return self->prefix_edges_(prefix);
    }

    static void on_legal_move(void* data, const char* word, int square, int direction) noexcept
    {
        auto* self = reinterpret_cast<Callbacks*>(data);
        return self->on_legal_move_(word, square, direction);
    }

    void clear_legal_moves() noexcept
    {
        legal_moves_.clear();
    }

    std::vector<Move> sorted_legal_moves() const noexcept
    {
        auto result = legal_moves_;
        std::sort(result.begin(), result.end());
        return result;
    }

    cicero_callbacks make_callbacks() const noexcept
    {
        cicero_callbacks cb;
        cb.onlegal = &Callbacks::on_legal_move;
        cb.getedges = &Callbacks::prefix_edges;
        cb.onlegaldata = this;
        cb.getedgesdata = this;
        return cb;
    }

private:
    cicero_edges prefix_edges_(const char* prefix) const noexcept
    {
        cicero_edges out;
        auto edges = mafsa_.get_edges(prefix);
        static_assert(sizeof(edges) == sizeof(out));
        memcpy(&out, &edges, sizeof(out));
        return out;
    }

    void on_legal_move_(const char* word, int square, int direction)
    {
        legal_moves_.emplace_back(word, square, direction);
    }

    Mafsa             mafsa_;
    std::vector<Move> legal_moves_;
};

Callbacks make_callbacks(const std::vector<std::string>& words = DICT)
{
    MafsaBuilder builder;
    for (const auto& word : words) {
        REQUIRE(builder.insert(word) == true);
    }
    auto maybe_mafsa = builder.finish();
    REQUIRE(static_cast<bool>(maybe_mafsa) == true);
    return Callbacks(std::move(*maybe_mafsa));
}

cicero_rack make_rack(std::string tiles)
{
    cicero_rack rack;
    memset(&rack, 0, sizeof(rack));
    for (char tile : tiles) {
        cicero_rack_add_tile(&rack, tile);
    }
    return rack;
}

TEST_CASE("Cicero first moves")
{
    const std::vector<std::string> words = {
        "UNIFY",
        "FRY",
        "APPLE",
        "ORANGE",
        "HELLO",
        "WORLD",
        "SULFURED",
    };

    auto cb = make_callbacks(words);
    cicero_movegen movegen;
    cicero_movegen_init(&movegen, cb.make_callbacks());

    SECTION("First move generate moves")
    {
        auto rack = make_rack("UNRYFIA");
        std::vector<Move> expect = {
            // horizontal FRY
            { "H6", "FRY" },
            { "H7", "FRY" },
            { "H8", "FRY" },
            // vertical FRY
            { "8F", "FRY" },
            { "8G", "FRY" },
            { "8H", "FRY" },

            // horizontal UNIFY
            { "H4", "UNIFY" },
            { "H5", "UNIFY" },
            { "H6", "UNIFY" },
            { "H7", "UNIFY" },
            { "H8", "UNIFY" },
            // vertical UNIFY
            { "8D", "UNIFY" },
            { "8E", "UNIFY" },
            { "8F", "UNIFY" },
            { "8G", "UNIFY" },
            { "8H", "UNIFY" },
        };
        std::sort(expect.begin(), expect.end());
        cicero_movegen_generate(&movegen, rack);
        auto legal_moves = cb.sorted_legal_moves();
        CHECK(legal_moves == expect);
    }

    SECTION("First move with blank tile")
    {
        auto rack = make_rack("APPL ZW");
        std::vector<Move> expect = {
            // horizontal APPLe
            { "H4", "APPLe" },
            { "H5", "APPLe" },
            { "H6", "APPLe" },
            { "H7", "APPLe" },
            { "H8", "APPLe" },
            // vertical APPLe
            { "8D", "APPLe" },
            { "8E", "APPLe" },
            { "8F", "APPLe" },
            { "8G", "APPLe" },
            { "8H", "APPLe" },
        };
        std::sort(expect.begin(), expect.end());
        cicero_movegen_generate(&movegen, rack);
        auto legal_moves = cb.sorted_legal_moves();
        CHECK(legal_moves == expect);
    }

    SECTION("Second move after H8 unify")
    {
        char tiles[5]   = {      'U',      'N',       'I',       'F',       'Y' };
        int  squares[5] = { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") };
        cicero_move move;
        move.tiles   = &tiles[0];
        move.squares = &squares[0];
        move.ntiles  = 5;
        move.direction = CICERO_HORZ;
        cicero_movegen_make_move(&movegen, &move);


        auto rack = make_rack("RDSELUU");
        Move expect{ "11E", "SULFURED" };
        cicero_movegen_generate(&movegen, rack);
        auto legal_moves = cb.sorted_legal_moves();
        REQUIRE(legal_moves.size() == 1);
        CHECK(legal_moves[0] == expect);
    }
}
