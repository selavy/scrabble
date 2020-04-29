#pragma once
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cicero/cicero.h>
#include <scrabble.h>
#include <mafsa++.h>
#include "test_data.h"


struct Callbacks
{
    using Move = scrabble::Move;

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

    bool isword(const std::string& word) const noexcept
    {
        return mafsa_.isword(word);
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
        legal_moves_.emplace_back();
        legal_moves_.back().square = scrabble::Square{square};
        legal_moves_.back().direction = static_cast<scrabble::Direction>(direction);
        legal_moves_.back().word = word;
        legal_moves_.back().score = -1;
    }

    Mafsa             mafsa_;
    std::vector<Move> legal_moves_;
};

inline Callbacks make_callbacks(const std::vector<std::string>& words = DICT)
{
    MafsaBuilder builder;
    for (const auto& word : words) {
        bool ok = builder.insert(word);
        assert(ok == true);
    }
    auto maybe_mafsa = builder.finish();
    assert(static_cast<bool>(maybe_mafsa) == true);
    return Callbacks(std::move(*maybe_mafsa));
}

inline cicero_rack make_rack(std::string tiles)
{
    cicero_rack rack;
    memset(&rack, 0, sizeof(rack));
    for (char tile : tiles) {
        cicero_rack_add_tile(&rack, tile);
    }
    return rack;
}

inline std::vector<scrabble::Move> make_move_list(const std::vector<std::string>& isc_specs)
{
    std::vector<scrabble::Move> result;
    for (const auto& isc_spec : isc_specs) {
        result.emplace_back(scrabble::Move::from_isc_spec(isc_spec));
    }
    std::sort(result.begin(), result.end());
    return result;
}

