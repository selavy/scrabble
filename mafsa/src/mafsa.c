#include <mafsa/mafsa.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include "iconv.h"

#include <stdio.h> // TEMP TEMP

typedef unsigned int uint;
typedef mafsa_node_  node;

int isterm(const uint* terms, int s)
{
    assert(0 <= s);
    size_t entry = (size_t)s / sizeof(uint);
    size_t shift = (size_t)s % sizeof(uint);
    uint   mask  = 1u << shift;
    return (int)(terms[entry] & mask) != 0;
}

int mafsa_isword(const struct mafsa *m, const char *const word)
{
    // printf("ISWORD: '%s'\n", word);
    const node *nodes = m->nodes;
    const int   size  = m->size;
    int s = 0;
    for (const char *p = word; *p != '\0'; ++p) {
        const int c = iconv(*p);
        const int t = nodes[s].children[c];
        assert(0 <= s && s < size);
        assert(0 <= c && c <= 26);
        // printf("\tch=%c c=%d t=%d\n", *p, c, t);
        if (t == 0) {
            return 0;
        }
        s = t;
    }
    // printf("Leaving term[%d] = %d (0x%08x)\n", s, isterm(m->terms, s), m->terms[s/sizeof(uint)]);
    return isterm(m->terms, s);
}

void mafsa_free(mafsa *m)
{
    free(m->nodes);
    free(m->terms);
#ifndef NDEBUG
    m->nodes = NULL;
    m->terms = NULL;
    m->size  = 0;
#endif
}
