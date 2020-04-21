#include "mafsa.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include "iconv.h"

typedef unsigned int uint;

int isterm(const uint* terms, int s)
{
    assert(0 <= s);
    size_t entry  = (size_t)s / sizeof(uint);
    size_t bitnum = (size_t)s % sizeof(uint);
    uint   mask   = 1u << bitnum;
    return (int)(terms[entry] & mask);
}

int mafsa_isword(const struct mafsa *m, const char *const word)
{
    const int  *children = m->children;
    const int   size     = m->size;
    int s = 0;
    for (const char *p = word; *p != '\0'; ++p) {
        assert(0 <= s && s < size);
        assert(0 <= *p && *p <= 26);
        const int c = iconv(*p);
        const int t = children[26*s + c];
        if (t == 0) {
            return 0;
        }
        s = t;
    }
    return isterm(m->terms, s);
}
