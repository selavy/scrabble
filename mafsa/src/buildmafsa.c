#include <mafsa/mafsa.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "iconv.h"
#include "khash.h"
#include "kvec.h"

#include <stdio.h> // TEMP TEMP


typedef unsigned int uint;
typedef mafsa_node   node;


static int expand(mafsa_builder *m, size_t amount)
{
    size_t capacity = (size_t)m->capacity + amount;
    node*  nodes    = realloc(m->nodes, sizeof(*nodes) * capacity);
    int*   terms    = realloc(m->terms, sizeof(*terms) * capacity);
    if (!nodes || !terms) {
        free(nodes);
        free(terms);
        return -1;
    }
    memset(&nodes[m->capacity], 0, sizeof(*nodes) * amount);
    memset(&terms[m->capacity], 0, sizeof(*terms) * amount);
    m->capacity = (int)capacity;
    m->nodes    = nodes;
    m->terms    = terms;
    return 0;
}

int mafsa_builder_start(mafsa_builder *m)
{
    m->capacity = 0;
    m->size     = 1;
    m->terms    = NULL;
    m->nodes    = NULL;
    return expand(m, 1);
}

#define TRY(x) if ((rc = (x)) != 0) { return rc; }

int mafsa_builder_insert(mafsa_builder *m, const char* const word)
{
    int rc;
    int s = 0;
    for (const char *p = word; *p != '\0'; ++p) {
        const int c = iconv(*p);
        assert(0 <= s && s < m->size);
        assert(0 <= c && c < 26);
        const int t = m->nodes[s].children[c];
        if (t == 0) {
            const int next = m->size++;
            if (m->size >= m->capacity) {
                TRY(expand(m, 10));
            }
            m->nodes[s].children[c] = next;
            m->terms[next] = 0;
            s = next;
        } else {
            s = t;
        }
    }
    assert(0 <= s && s < m->size);
    m->terms[s] = 1;
    return 0;
}

KHASH_MAP_INIT_INT(m32, int);

typedef kvec_t(int) vec_t;
typedef khash_t(m32) ht_t;
struct reduce_state
{
    vec_t reg;
    ht_t *rep;
};
typedef struct reduce_state reduce_state;

int nodecmp(const mafsa_builder *m, int s, int t)
{
    // from https://www.aclweb.org/anthology/J00-1002.pdf pg. 7:
    //
    // (assuming doing post-order traversal of the trie)
    //
    // State `p` belongs to the same (equivalence) class as `q` if and only if:
    //
	// 1. they are either both final or both nonfinal; and
	// 2. they have the same number of outgoing transitions; and
	// 3. corresponding outgoing transitions have the same labels; and
    // 4'. corresponding transitions lead to the same states.

    // TODO: do I need to verify that the value is the same?
    if (m->terms[s] != m->terms[t]) {
        return 1;
    }
    return memcmp(&m->nodes[s], &m->nodes[t], sizeof(*m->nodes));
}

void visit_post(mafsa_builder *m, int s, reduce_state *state)
{
    int ret;
    khiter_t k;
    vec_t *reg = &state->reg;
    ht_t  *rep = state->rep;

    assert(s < m->size);
    for (int i = 0; i < 26; ++i) {
        const int t = m->nodes[s].children[i];
        if (t != 0) {
            visit_post(m, t, state);
        }
    }

    {
        for (int i = 0; i < 26; ++i) {
            const int t = m->nodes[s].children[i];
            if (t == 0) {
                continue;
            }
            k = kh_get(m32, rep, t);
            if (k == kh_end(rep)) {
                continue;
            }
            m->nodes[s].children[i] = kh_value(rep, k);
        }

        if (s == 0) {
            return;
        }

        for (int i = 0; i < kv_size(*reg); ++i) {
            const int t = kv_A(*reg, i);
            assert(kh_get(m32, rep, t) == kh_end(rep));
            if (nodecmp(m, s, t) == 0) {
                k = kh_put(m32, rep, s, &ret);
                assert(k != kh_end(rep));
                assert(ret != -1 && ret != 0);
                kh_value(rep, k) = t;
                return;
            }
        }

        kv_push(int, *reg, s);
    }
}

void reduce(mafsa_builder *m)
{
    int ret;
    khiter_t k;
    reduce_state state;

    kv_init(state.reg);
    state.rep = kh_init(m32);
    visit_post(m, 0, &state);

    // re-number states
    // TODO: use flat array instead of hash table
    int new_size = 0;
    ht_t *conv  = kh_init(m32);
    ht_t *rconv = kh_init(m32);
    for (int i = 0; i < m->size; i++) {
        if (kh_get(m32, state.rep, i) != kh_end(state.rep)) {  // dead node
            continue;
        }
        k = kh_put(m32, conv, i, &ret);
        assert(ret != 0); // TODO: handle error
        kh_value(conv, k) = new_size;
        k = kh_put(m32, rconv, new_size, &ret);
        assert(ret != 0); // TODO: handle error
        kh_value(rconv, k) = i;
        ++new_size;
    }
    kh_destroy(m32, state.rep);
    kv_destroy(state.reg);

    node *new_nodes = malloc(sizeof(*new_nodes) * new_size);
    int  *new_terms = malloc(sizeof(*new_terms) * new_size);
    memset(new_nodes, 0, sizeof(*new_nodes) * new_size);
    memset(new_terms, 0, sizeof(*new_terms) * new_size);
    assert(new_nodes != NULL); // TODO: handle error
    for (int i = 0; i < new_size; ++i) {
        assert(kh_get(m32, rconv, i) != kh_end(rconv));
        const int newidx = i;
        const int oldidx = kh_value(rconv, kh_get(m32, rconv, newidx));
        new_terms[newidx] = m->terms[oldidx];
        for (int child = 0; child < 26; ++child) {
            const int old_t = m->nodes[oldidx].children[child];
            if (old_t == 0) {
                continue;
            }
            assert(kh_get(m32, conv, old_t) != kh_end(conv));
            const int new_t = kh_value(conv, kh_get(m32, conv, old_t));
            new_nodes[newidx].children[child] = new_t;
        }
    }
    kh_destroy(m32, conv);
    kh_destroy(m32, rconv);

    free(m->nodes);
    free(m->terms);
    m->nodes = new_nodes;
    m->terms = new_terms;
    m->size  = new_size;
    m->capacity = new_size;
}

int mafsa_builder_finish(mafsa_builder *m, mafsa *out)
{
    reduce(m);
    size_t size  = (size_t)m->size;
    node  *nodes = malloc(sizeof(*nodes) * size);
    uint  *terms = malloc(size + sizeof(*terms));
    if (!nodes || !terms) {
        free(nodes);
        free(terms);
        return -1;
    }
    memset(nodes, 0, sizeof(*nodes) * size);
    memset(terms, 0, size + sizeof(*terms));
    for (size_t i = 0; i < size; ++i) {
        memcpy(&nodes[i], &m->nodes[i], sizeof(*nodes));
        size_t entry = (size_t)i / sizeof(uint);
        size_t shift = (size_t)i % sizeof(uint);
        uint   tbit  = m->terms[i] ? 1u : 0u;
        // printf("i=%zu entry=%zu shift=%zu size=%zu\n", i, entry, shift, size);
        terms[entry] |= tbit << shift;
    }
    out->nodes = nodes;
    out->terms = terms;
    out->size  = m->size;
    free(m->nodes);
    free(m->terms);
    m->nodes = NULL;
    m->terms = NULL;
    m->size = 0;
    return 0;
}
