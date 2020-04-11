#include <iostream>
#include <cassert>
#include <vector>
#include <cstdlib>
#include <cstring>


#define xassert(c, fmt, ...)                                              \
    do {                                                                  \
        if (!(c)) {                                                       \
            fprintf(stderr, "ASSERT: " #c " : " fmt "\n", ##__VA_ARGS__); \
            assert(c);                                                    \
        }                                                                 \
    } while (0)

// ----------------------------------------------------------------------------
// EngineTrie
// ----------------------------------------------------------------------------
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iosfwd>
#include <cassert>
#include <algorithm>

struct EngineTrie
{
    struct Node {
        char                value    = ' '; // TEMP TEMP
        bool                terminal = false;
        std::map<char, int> children;
        int                 base = 0;
    };
    EngineTrie() {
        nodes.emplace_back();
    }
    void insert(const char* word)
    {
        int curr = 0;
        const char* c = word;
        while (*c != '\0') {
            char ch = EngineTrie::safech(*c);
            symbols.insert(ch);
            assert(curr < nodes.size());
            auto& node = nodes[curr];
            auto it = node.children.find(ch);
            if (it != node.children.end()) {
                curr = it->second;
            } else {
                int next = static_cast<int>(nodes.size());
                nodes.emplace_back();
                nodes.back().value = ch; // TEMP TEMP
                nodes[curr].children[ch] = next;
                curr = next;
            }
            ++c;
        }
        nodes[curr].terminal = true;
    }
    void insert(const std::string& w) { insert(w.c_str()); }
    bool is_word(const char* word) const
    {
        int curr = 0;
        const char* c = word;
        while (*c != '\0') {
            char ch = EngineTrie::safech(*c);
            assert(curr < nodes.size());
            auto& node = nodes[curr];
            auto it = node.children.find(ch);
            if (it == node.children.end()) {
                return false;
            }
            curr = it->second;
            ++c;
        }
        return nodes[curr].terminal;
    }
    bool is_word(const std::string& w) const { return is_word(w.c_str()); }
    std::vector<char> children(const char* prefix) const
    {
        [[maybe_unused]] bool unused;
        return children(prefix, unused);
    }
    std::vector<char> children(const char* prefix, bool& is_terminal) const
    {
        std::vector<char> result;
        int curr = 0;
        const char* c = prefix;
        while (*c != '\0') {
            char ch = EngineTrie::safech(*c);
            assert(curr < nodes.size());
            auto& node = nodes[curr];
            auto it = node.children.find(ch);
            if (it == node.children.end()) {
                is_terminal = false;
                return result;
            }
            curr = it->second;
            ++c;
        }
        for (auto [tile, children] : nodes[curr].children) {
            result.push_back(tile);
        }
        is_terminal = nodes[curr].terminal;
        return result;
    }
    static constexpr char safech(char c) noexcept {
        if ('A' <= c && c <= 'Z') {
            return c;
        } else if ('a' <= c && c <= 'z') {
            return 'A' + (c - 'a');
        } else {
            assert(0 && "invalid character");
        }
    }

    template <class F>
    void visit(F&& f) {
        std::string word;
        visit_(f, 0, word);
    }

    template <class F>
    void visit_(F&& f, int curr, std::string& word) {
        f(nodes[curr], nodes, word);
        for (auto [ch, child] : nodes[curr].children) {
            word += ch;
            visit_(f, child, word);
            word.pop_back();
        }
    }

    Node* getNext(int x) { return x < nodes.size() ? &nodes[x] : nullptr; }
    int num_states() const { return static_cast<int>(nodes.size()); }
    int num_transitions() const { return std::max(num_states() - 1, 0); }
    int num_symbols() const { return static_cast<int>(symbols.size()); }

    using Nodes = std::vector<Node>;
    Nodes nodes;
    int   n_transitions;
    std::set<char> symbols;
};

// ----------------------------------------------------------------------------

int iconv(char c) {
    if ('a' <= c && c <= 'z') {
        return c - 'a' + 1;
    } else if ('A' <= c && c <= 'Z') {
        return c - 'A' + 1;
    } else {
        assert(0 && "invalid character");
        return -1; // should cause an error
    }
}

using Array = std::vector<int>;

bool walk3(const Array& base, const Array& next, const Array& chck, const Array& term, const std::string& word)
{
    int s = 0;
    for (const char ch : word) {
        const int c = iconv(ch);
        assert(s >= 0);
        if (s >= base.size()) {
            return false;
        }
        assert(base[s] + c >= 0);
        if (base[s] + c > chck.size()) {
            return false;
        }
        assert(0 <= (base[s] + c) && (base[s] + c) < next.size());
        if (chck[base[s] + c] != s) {
            return false;
        }
        const int t = next[base[s] + c];
        s = t;
    }
    return s < term.size() && term[s];
}

// IDEA: use top bit in index to indicate if terminal? or separate array?
int main(int argc, char** argv)
{
    // clang-format off
    const std::vector<std::string> words = {
        "HE",
        "HEAT",
        "HEAL",
        "HEAP",
        "HEM",
        "HA",
        "HAT",
        "HATE",
        "HAPPY",
        "HI",
        "HIM",
        "HIP",
        "HIPPY",
        "HIT",
    };
    using Nodes = EngineTrie::Nodes;
    using Node  = Nodes::value_type;
    EngineTrie et;
    for (auto& word : words) {
        et.insert(word);
    }

    // ------------------------------------------------------------------------
    // build
    // ------------------------------------------------------------------------

    int n_symbols     = et.num_symbols();
    int n_states      = et.num_states();
    Array base(n_states, -1);
    Array term(n_states, 0);
    Array next(n_symbols * n_states, -1);
    Array chck(n_symbols * n_states, -1);

    { // TEMP TEMP
        int nodenum = 0;
        et.visit([&](Node& node, Nodes& nodes, const std::string& word) { node.base = nodenum++; });
    }

    int nodenum = 0;
    et.visit([&](Node& node, Nodes& nodes, const std::string& word)
        {
            // get all child transitions
            std::vector<int> cs;
            for (auto&& [ch, link] : node.children) {
                cs.push_back(iconv(ch));
            }

            // find an `i` in `next` s.t. `next[i+c] == -1` for all `c` in `cs`
            int new_base = -1;
            for (int i = 0; i < next.size(); ++i) {
                if (std::all_of(std::begin(cs), std::end(cs), [&](int c) { return i+c < next.size() && next[i+c] == -1; })) {
                    new_base = i;
                    break;
                }
            }
            if (new_base == -1) {
                printf("Unable to find a new_base for '%c'\n", node.value);
                assert(0);
            }

            // install transitions in table
            assert(0 <= nodenum && nodenum < base.size());
            assert(0 <= nodenum && nodenum < term.size());
            base[nodenum] = new_base;
            term[nodenum] = node.terminal ? 1 : 0;
            for (auto&& [ch, link] : node.children) {
                const int c = iconv(ch);
                assert(0 <= (new_base + c) && (new_base + c) < next.size());
                assert(0 <= (new_base + c) && (new_base + c) < chck.size());
                assert(next[new_base + c] == -1);
                assert(chck[new_base + c] == -1);
                next[new_base + c] = nodes[link].base;
                chck[new_base + c] = nodenum;
            }
            ++nodenum;
        });

    // ------------------------------------------------------------------------
    // verification
    // ------------------------------------------------------------------------

    int failures = 0;
    for (auto w : words) {
        if (!walk3(base, next, chck, term, w)) {
            printf("Failed to find: '%s'\n", w.c_str());
            ++failures;
        }
    }

    printf("\n\n");
    if (failures == 0) {
        printf("Passed.\n");
    } else {
        printf("Failed.\n");
    }

    return 0;
}

