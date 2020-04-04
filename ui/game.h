#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

using Dict = std::unordered_set<std::string>;

struct Word {
    // blank or not            => 1 bit / letter for blank or not
    // 26 letters + 1 empty(?) => 5 bits / letter
    // 1-15 length             => 4 bits for length

    constexpr static int MaxWordLength = 15;
    using Letters = std::array<char, MaxWordLength>;
    // clang-format off
    constexpr static Letters all_blank = {
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    };
    // clang-format on

    Letters letters = all_blank;
    int length = 0;

    constexpr Word(const std::string& word) noexcept {
        assert(word.size() <= static_cast<std::size_t>(MaxWordLength));
        length = static_cast<int>(word.size());
        // std::fill(std::begin(letters), std::end(letters), ' ');
        for (std::size_t i = 0; i < word.size(); ++i) {
            assert('A' <= word[i] && word[i] <= 'Z');
            letters[i] = word[i];
        }
    }

    Word(const Word& other) noexcept : letters{other.letters}, length{other.length} {}

    Word(Word&& other) noexcept : letters{other.letters}, length{other.length} {
        std::fill(std::begin(other.letters), std::end(other.letters), ' ');
        other.length = 0;
    }

    constexpr int size() const noexcept { return length; }

    constexpr std::size_t hash() const noexcept {
        uint64_t x1 = 0;
        uint64_t x2 = 0;
        for (size_t i = 0; i < 8; i++) {
            x1 ^= (static_cast<uint64_t>(letters[0 + i]) << (i * 8));
        }
        for (size_t i = 0; i < 7; i++) {
            x2 ^= (static_cast<uint64_t>(letters[8 + i]) << (i * 8));
        }
        return (x1 ^ x2) + 0x9e779b9 + (x1 << 6) + (x2 >> 2);
    }

    bool operator==(Word other) const noexcept {
        return memcmp(&this->letters[0], &other.letters[0], letters.size() * sizeof(char)) == 0;
    }

    bool operator!=(Word other) const noexcept { return !(*this == other); }
};

std::ostream& operator<<(std::ostream& os, const Word& word) {
    assert(word.length <= Word::MaxWordLength);
    os << '\'';
    for (int i = 0; i < word.length; ++i) {
        os << word.letters[i];
    }
    os << '\'';
    return os;
}

namespace std {
template <>
struct hash<Word> {
    std::size_t operator()(const Word& w) const noexcept { return w.hash(); }
};
}  // namespace std

// std::unique_ptr<Dict> load_dictionary(const char* filename)
// {
//     std::ifstream ifs(filename);
//     if (!ifs) {
//         return nullptr;
//     }
//     auto dict = std::make_unique<Dict>();
//     std::string word;
//     while (ifs >> word) {
//         if (word.empty() || word.size() > 7) {
//             continue;
//         }
//         dict->emplace(std::move(word));
//     }
//     return dict;
// }
//
// bool is_word(const Dict& dict, char word[7])
// {
//     std::string key;
//     for (size_t i = 0; i < word.size(); ++i) {
//         auto c = word[i];
//         if ('A' <= c && c <= 'Z') {
//             key += c;
//         } else if ('a' <= c && c <= 'z') {
//             key += c;
//         } else {
//             assert(c == ' ');
//             break;
//         }
//     }
//     return dict.find(key) != dict.end();
// }
