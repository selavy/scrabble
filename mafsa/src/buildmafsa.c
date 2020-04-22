#include <mafsa/mafsa.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "iconv.h"
#include <stdio.h>

typedef unsigned int uint;

static int expand(mafsa_builder *m, size_t amount)
{
    size_t capacity = (size_t)m->capacity + 26*amount;
    int*   children = realloc(m->children, sizeof(int) * capacity);
    int*   terms    = realloc(m->terms,    sizeof(int) * capacity);
    if (!children || !terms) {
        free(children);
        free(terms);
        return -1;
    }
    memset(&children[m->capacity], 0, sizeof(int) * 26*amount);
    memset(&terms   [m->capacity], 0, sizeof(int) * 26*amount);
    m->capacity = (int)capacity;
    m->children = children;
    m->terms    = terms;
    return 0;
}


int mafsa_builder_start(mafsa_builder *m)
{
    m->capacity = 0;
    m->size     = 1;
    m->terms    = NULL;
    m->children = NULL;
    return expand(m, 1);
}

#define TRY(x) if ((rc = (x)) != 0) { return rc; }

int mafsa_builder_insert(mafsa_builder *m, const char* const word)
{
    int rc;
    int s = 0;
    for (const char *p = word; *p != '\0'; ++p) {
        const int c = iconv(*p);
        const int i = 26*s + c;
        assert(0 <= s && s < m->size);
        assert(0 <= c && c < 26);
        assert(i < m->capacity);
        assert(i < 26*m->size);
        const int t = m->children[i];
        if (t == 0) {
            const int next = m->size++;
            m->children[i] = next;
            m->terms[i]    = 0;
            if (26*m->size >= m->capacity) {
                TRY(expand(m, 10));
            }
            s = next;
        } else {
            s = t;
        }
    }
    assert(0 <= s && s < m->size);
    m->terms[26*s] = 1;
    return 0;
}

int mafsa_builder_finish(mafsa_builder *m, mafsa *out)
{
    // TODO: reduce
    size_t size = (size_t)m->size * 26;
    int   *children = malloc(sizeof(int) * size);
    uint  *terms    = malloc(size + sizeof(uint));
    if (!children || !terms) {
        free(children);
        free(terms);
        return -1;
    }
    memset(terms, 0, size);
    for (size_t i = 0; i < size; ++i) {
        children[i] = m->children[i];
        size_t entry = (size_t)i / sizeof(uint);
        size_t shift = (size_t)i % sizeof(uint);
        uint   tbit  = m->terms[i] ? 1u : 0u;
        // printf("i=%zu entry=%zu shift=%zu size=%zu\n", i, entry, shift, size);
        terms[entry] |= tbit << shift;
    }
    out->children = children;
    out->terms    = terms;
    out->size     = m->size;
    free(m->children);
    free(m->terms);
    return 0;
}
