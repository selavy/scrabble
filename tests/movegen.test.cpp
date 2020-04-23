#include <catch2/catch.hpp>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cicero/cicero.h>
#include "mafsa.h"
#include "test_data.h"

struct Move
{
    Move(std::string w, int s, int d) noexcept
        : word{std::move(w)}, square{s}, direction{d} {}

    std::string word;
    int         square;
    int         direction;
};

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

    const std::vector<Move> legal_moves() const noexcept
    {
        return legal_moves_;
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

TEST_CASE("Cicero first test")
{
    auto cb = make_callbacks();
    cicero_movegen movegen;
    cicero_movegen_init(&movegen, cb.make_callbacks());
}
