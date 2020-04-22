#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct mafsa_node { int children[26]; };
typedef struct mafsa_node mafsa_node;

struct mafsa
{
    mafsa_node   *nodes;
    unsigned int *terms;
    int           size;
};
typedef struct mafsa mafsa;

extern int  mafsa_isword(const mafsa *m, const char *const word);
extern void mafsa_free(mafsa *m);

struct mafsa_builder
{
    mafsa_node *nodes;
    int        *terms;
    int         next;
    int         size;
    int         capacity;
};
typedef struct mafsa_builder mafsa_builder;

extern int mafsa_builder_start (mafsa_builder *m);
extern int mafsa_builder_insert(mafsa_builder *m, const char *const word);
extern int mafsa_builder_finish(mafsa_builder *m, mafsa *out);

#ifdef __cplusplus
}
#endif
