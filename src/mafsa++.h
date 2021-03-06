#pragma once

#include <optional>
#include <mafsa/mafsa.h>
#include <memory>
#include <climits>
#include "mafsa_generated.h"


struct Mafsa
{
    explicit Mafsa(mafsa&& m) noexcept : mafsa_{m} {}
    Mafsa(Mafsa&& other) noexcept : mafsa_{other.mafsa_}
    {
        other.mafsa_.nodes = nullptr;
        other.mafsa_.terms = nullptr;
        other.mafsa_.size  = 0;
    }
    ~Mafsa() noexcept;
    const mafsa& operator*() const  { return mafsa_; }
    const mafsa* operator->() const { return &mafsa_; }
    bool isword(const std::string& word) const noexcept;
    bool isterm(int s) const noexcept;
    mafsa_edges get_edges(const char* const word) const noexcept;
    mafsa_edges get_edges(const std::string& word) const noexcept { return get_edges(word.c_str()); }
    static std::optional<Mafsa> load(const std::string& filename);

private:
    Mafsa() : mafsa_{} {}
    mafsa mafsa_;
};


struct MafsaBuilder
{
    MafsaBuilder()
    {
        builder.nodes = nullptr;
        builder.terms = nullptr;
        if (mafsa_builder_start(&builder) != 0) {
            throw std::runtime_error("unable to initialize MA-FSA builder");
        }
    }

    ~MafsaBuilder()
    {
        free(builder.nodes);
        free(builder.terms);
    }

    bool insert(const std::string word)
    {
        return mafsa_builder_insert(&builder, word.c_str()) == 0;
    }

    // TODO: return Mafsa instead
    std::optional<Mafsa> finish()
    {
        mafsa out;
        if (mafsa_builder_finish(&builder, &out) != 0) {
            return std::nullopt;
        }
        return Mafsa{std::move(out)};
    }

    static std::optional<Mafsa> build_from_file(const std::string& filename, int max_words=INT_MAX);

    mafsa_builder builder;
};
