#include "mafsa.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include "iconv.h"

#include <stdio.h> // TEMP TEMP

typedef unsigned int uint;

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
    printf("ISWORD: '%s'\n", word);
    const int  *children = m->children;
    const int   size     = m->size;
    int s = 0;
    for (const char *p = word; *p != '\0'; ++p) {
        const int c = iconv(*p);
        assert(0 <= s && s < size);
        assert(0 <= c && c <= 26);
        const int t = children[26*s + c];
        printf("\tch=%c c=%d t=%d\n", *p, c, t);
        if (t == 0) {
            return 0;
        }
        s = t;
    }
    return isterm(m->terms, s);
}

void mafsa_free(mafsa *m)
{
    free(m->children);
    free(m->terms);
    m->children = NULL;
    m->terms    = NULL;
    m->size     = 0;
}
