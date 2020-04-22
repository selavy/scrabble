#pragma once

#include <optional>
#include <mafsa/mafsa.h>
#include <memory>
#include "mafsa_generated.h"


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

    std::optional<mafsa> finish()
    {
        mafsa out;
        if (mafsa_builder_finish(&builder, &out) != 0) {
            return std::nullopt;
        }
        return out;
    }

    mafsa_builder builder;
};

struct Mafsa
{
    static std::optional<Mafsa> load(const std::string& filename);

    Mafsa(mafsa m_)
        : m{std::make_unique<mafsa>()}
    {
        *m = m_;
    }

    Mafsa(Mafsa&& other) noexcept : m{std::move(other.m)} {}

    ~Mafsa()
    {
        if (m) {
            mafsa_free(m.release());
        }
    }

    bool isword(const std::string& word) const noexcept
    {
        return mafsa_isword(m.get(), word.c_str());
    }

    bool isterm(int s) const noexcept
    {
        return mafsa_isterm(m.get(), s) != 0;
    }

    std::unique_ptr<mafsa> m;

private:
    Mafsa() : m{std::make_unique<mafsa>()} {}

};
