#include <cstdio>
#include <cassert>
#include <vector>
#include <string>
#include "mafsa.h"

int main([[maybe_unused]]int argc, [[maybe_unused]] const char** argv) {
    const std::vector<std::string> words = {
        "HELLO",
        "GOODBYE",
    };

    int rc;
    mafsa_builder builder;

    rc = mafsa_builder_start(&builder);
    if (rc != 0) {
        fprintf(stderr, "error: failed to initialize builder\n");
        return 1;
    }

    for (const auto& word : words) {
        rc = mafsa_builder_insert(&builder, word.c_str());
        if (rc != 0) {
            fprintf(stderr, "error: failed to insert word: '%s'\n", word.c_str());
            return 1;
        }
    }

    mafsa m;
    rc = mafsa_builder_finish(&builder, &m);
    if (rc != 0) {
        fprintf(stderr, "error: failed to finalize builder\n");
        return 1;
    }

    assert(mafsa_isword(&m, "HELLO")   == true);
    assert(mafsa_isword(&m, "GOODBYE") == true);
    assert(mafsa_isword(&m, "GOODBY")  == false);
    assert(mafsa_isword(&m, "HELLOO")  == false);
    assert(mafsa_isword(&m, "HELL")    == false);

    mafsa_free(&m);

    return 0;
}
