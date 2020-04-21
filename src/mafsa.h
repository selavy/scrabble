#pragma once


struct mafsa
{
    int          *children;
    unsigned int *terms;
    int           size;
};

int mafsa_isword(const struct mafsa *m, const char *const word);
