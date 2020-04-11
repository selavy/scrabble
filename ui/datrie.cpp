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

struct DATrie3
{
    Array base; // top 1 of base[s] indicates terminal or not
    Array next;
    Array chck;
};

int getbase3(const DATrie3& trie, int s)
{
    return s < trie.base.size() ? trie.base[s] & ~(1u << 31) : 0;
}

bool getterm3(const DATrie3& trie, int s)
{
    return s < trie.base.size() ? (trie.base[s] >> 31) != 0 : false;
}

bool walk3(const DATrie3& trie, const std::string& word)
{
    auto& base = trie.base;
    auto& next = trie.next;
    auto& chck = trie.chck;

    int s = 0;
    for (const char ch : word) {
        const int c = iconv(ch);
        assert(s >= 0);
        int base_s = getbase3(trie, s);
        if (base_s + c > chck.size() || chck[base_s + c] != s) {
            return false;
        }
        assert(0 <= (base_s + c) && (base_s + c) < next.size());
        s = next[base_s + c];
    }
    return getterm3(trie, s);
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

    const std::vector<std::string> missing = {
        "HELLO",
        "HEE",
        "HITE",
        "MISSING",
        "ANOTHER",
        "Z",
        "ZZZ",
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
    DATrie3 trie;
    trie.base = Array(n_states, -1);
    trie.next = Array(n_symbols * n_states, -1);
    trie.chck = Array(n_symbols * n_states, -1);

    { // TEMP TEMP
        int nodenum = 0;
        et.visit([&](Node& node, Nodes& nodes, const std::string& word) { node.base = nodenum++; });
    }

    et.visit([&](Node& node, Nodes& nodes, const std::string& word)
        {
            auto& base = trie.base;
            auto& next = trie.next;
            auto& chck = trie.chck;

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
            // TODO: implement relocate logic
            if (new_base == -1) {
                printf("Unable to find a new_base for '%c'\n", node.value);
                assert(0);
            }

            // install transitions in table
            const int nodenum = node.base;
            assert(0 <= nodenum && nodenum < base.size());
            base[nodenum] = new_base | ((node.terminal ? 1 : 0) << 31);
            for (auto&& [ch, link] : node.children) {
                const int c = iconv(ch);
                assert(0 <= (new_base + c) && (new_base + c) < next.size());
                assert(0 <= (new_base + c) && (new_base + c) < chck.size());
                assert(next[new_base + c] == -1);
                assert(chck[new_base + c] == -1);
                next[new_base + c] = nodes[link].base;
                chck[new_base + c] = nodenum;
            }
        });

    // ------------------------------------------------------------------------
    // verification
    // ------------------------------------------------------------------------

    int failures = 0;
    for (const auto& w : words) {
        if (!walk3(trie, w)) {
            printf("Failed to find: '%s'\n", w.c_str());
            ++failures;
        }
    }

    for (const auto& w : missing) {
        if (walk3(trie, w)) {
            printf("Accidentally found: '%s'\n", w.c_str());
            ++failures;
        }
    }
    if (failures == 0) {
        printf("Passed.\n");
    } else {
        printf("\nFailed.\n");
    }

    return 0;
}

