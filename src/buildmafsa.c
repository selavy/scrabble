#include "mafsa.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "iconv.h"

typedef unsigned int uint;

int mafsa_builder_start(mafsa_builder *m)
{
    size_t capacity = 100;
    m->capacity = (int)capacity;
    m->children = malloc(sizeof(int) * capacity * 26);
    m->terms    = malloc(sizeof(int) * capacity);
    m->size     = 1;
    memset(m->children, 0, sizeof(int) * capacity * 26);
    memset(m->terms,    0, sizeof(int) * capacity);
    return 0;
}

static int expand(mafsa_builder *m, size_t amount)
{
    size_t capacity = (size_t)m->capacity + amount;
    int*   children = realloc(m->children, sizeof(int) * capacity * 26);
    int*   terms    = realloc(m->terms,    sizeof(int) * capacity);
    if (!children || !terms) {
        free(children);
        free(terms);
        return -1;
    }
    memset(&children[m->capacity], 0, sizeof(int) * amount);
    memset(&terms   [m->capacity], 0, sizeof(int) * amount);
    m->capacity = (int)capacity;
    m->children = children;
    m->terms    = terms;
    return 0;
}

#define TRY(x) if ((rc = (x)) != 0) { return rc; }

int mafsa_builder_insert(mafsa_builder *m, const char* const word)
{
    int rc;
    int s = 0;
    for (const char *p = word; *p != '\0'; ++p) {
        assert(0 <= s && s < m->size);
        const int c = iconv(*p);
        const int i = 26*s + c;
        assert(0 <= c && c < 26);
        assert(i < m->capacity);
        assert(i < 26*m->size);
        const int t = m->children[i];
        if (t == 0) {
            const int next = m->size++;
            m->children[i] = next;
            m->terms[i]    = 0;
            if (m->size >= m->capacity) {
                TRY(expand(m, 26*10));
            }
            s = next;
        } else {
            s = t;
        }
    }
    assert(0 <= s && s < m->size);
    m->terms[s] = 1;
    return 0;
}

int mafsa_builder_finish(const mafsa_builder *m, mafsa *out)
{
    // TODO: reduce
    size_t size = (size_t)m->size;
    int   *children = malloc(sizeof(int) * 26 * size);
    uint  *terms    = malloc(size);
    if (!children || !terms) {
        free(children);
        free(terms);
        return -1;
    }
    memset(terms, 0, size);
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < 26; ++j) {
            children[i+j] = m->children[i+j];
        }
        size_t entry = (size_t)i / sizeof(uint);
        size_t shift = (size_t)i % sizeof(uint);
        terms[entry] |= (uint)(m->terms[i]?1:0) << shift;
    }
    out->children = children;
    out->terms    = terms;
    out->size     = m->size;
    return 0;
}
