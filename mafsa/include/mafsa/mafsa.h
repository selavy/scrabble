#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// TODO: add allocate / deallocate API for custom allocators

struct mafsa_node_ { int children[26]; };
typedef struct mafsa_node_ mafsa_node_;

struct mafsa
{
    mafsa_node_  *nodes;
    unsigned int *terms;
    int           size;
};
typedef struct mafsa mafsa;

struct mafsa_edges
{
    int  terminal;
    char edges[27]; // [A-Z], null-terminated
};
typedef struct mafsa_edges mafsa_edges;

extern int  mafsa_isword(const mafsa *m, const char *const word);
extern int  mafsa_isterm(const mafsa *m, int s);
extern void mafsa_free(mafsa *m);
extern mafsa_edges mafsa_prefix_edges(const mafsa *m, const char *const word);


struct mafsa_builder
{
    mafsa_node_ *nodes;
    int         *terms;
    int          size;
    int          capacity;
};
typedef struct mafsa_builder mafsa_builder;

extern int mafsa_builder_start (mafsa_builder *m);
extern int mafsa_builder_insert(mafsa_builder *m, const char *const word);
extern int mafsa_builder_finish(mafsa_builder *m, mafsa *out);

#ifdef __cplusplus
}
#endif
