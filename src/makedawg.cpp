#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <climits>
#include <cassert>
#include <mafsa/mafsa.h>

struct MafsaBuilder
{
    MafsaBuilder()
    {
        builder.nodes = nullptr;
        builder.terms = nullptr;
        if (mafsa_builder_start(&builder) != 0) {
            throw std::runtime_error("unable to initialize MA-FSA builder");
        }
    }

    ~MafsaBuilder()
    {
        free(builder.nodes);
        free(builder.terms);
    }

    bool insert(const std::string word)
    {
        return mafsa_builder_insert(&builder, word.c_str()) == 0;
    }

    std::optional<mafsa> finish()
    {
        mafsa out;
        if (mafsa_builder_finish(&builder, &out) != 0) {
            return std::nullopt;
        }
        return out;
    }

    mafsa_builder builder;
};

struct Mafsa
{
    Mafsa(mafsa m_)
        : m{m_}
    {}

    ~Mafsa()
    {
        mafsa_free(&m);
    }

    bool isword(const std::string& word) const noexcept
    {
        return mafsa_isword(&m, word.c_str());
    }

    mafsa m;
};


std::optional<Mafsa> load_dictionary(std::string path, int max_words)
{
    MafsaBuilder builder;
    std::string word;
    std::ifstream ifs{path};
    int n_words = 0;
    if (!ifs) {
        std::cerr << "error: unable to open input file\n";
        return std::nullopt;
    }
    while (ifs >> word) {
        if (word.empty()) {
            continue;
        }
        if (word.size() < 2 || word.size() > 15) {
            std::cerr << "warning: skipping invalid word: \"" << word << "\"\n";
        }
        bool valid_word = true;
        for (std::size_t i = 0; i < word.size(); ++i) {
            char c = word[i];
            if ('a' <= c && c <= 'z') {
                word[i] = static_cast<char>((c - 'a') + 'A');
            } else if ('A' <= c && c <= 'Z') {
                word[i] = c;
            } else {
                std::cerr << "warning: invalid character '" << c << "' in word \"" << word << "\"\n";
                valid_word = false;
                break;
            }
        }
        if (valid_word) {
            if (!builder.insert(word)) {
                std::cerr << "error: unable to insert word: " << word << "\n";
                return std::nullopt;
            }
            if (++n_words >= max_words) {
                break;
            }
        }
    }
    return builder.finish();
}

bool test_dictionary(const Mafsa& dict, std::string path, int max_words)
{
    std::string word;
    std::ifstream ifs{path};
    int n_words = 0;
    if (!ifs) {
        std::cerr << "error: unable to open input file\n";
        return false;
    }
    while (ifs >> word) {
        if (word.empty()) {
            continue;
        }
        if (word.size() < 2 || word.size() > 15) {
            std::cerr << "warning: skipping invalid word: \"" << word << "\"\n";
        }
        bool valid_word = true;
        for (std::size_t i = 0; i < word.size(); ++i) {
            char c = word[i];
            if ('a' <= c && c <= 'z') {
                word[i] = static_cast<char>((c - 'a') + 'A');
            } else if ('A' <= c && c <= 'Z') {
                word[i] = c;
            } else {
                std::cerr << "warning: invalid character '" << c << "' in word \"" << word << "\"\n";
                valid_word = false;
                break;
            }
        }
        if (valid_word) {
            if (!dict.isword(word)) {
                std::cerr << "Failed on word: " << word << "\n";
                return false;
            }
            if (++n_words >= max_words) {
                break;
            }
        }
    }
    return true;
}

std::string make_out_filename(std::string inname, std::string newext)
{
    const std::string ext = ".txt";
    auto pos = inname.rfind(ext);
    if (pos == std::string::npos) {
        return inname + newext;
    } else {
        assert(pos < inname.size());
        return inname.replace(pos, ext.size(), newext);
    }
}

#if 0
bool write_data(const std::string& filename, const uint8_t* buf, std::size_t length)
{
    std::ofstream ofs;
    ofs.open(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(buf), length);
    ofs.close();
    return true;
}
#endif

int main(int argc, char** argv)
{
    // TODO: real command line parser
    if (argc == 0) {
        std::cerr << "Usage: " << argv[0] << " [FILE]" << std::endl;
        return 0;
    }

    const std::string inname    = argc >= 2 ? argv[1]       : "";
    const int         max_words = argc >= 3 ? atoi(argv[2]) : INT_MAX;
    const std::string outname   = argc >= 4 ? argv[3]       : make_out_filename(inname, ".dict");

    std::cout << "INPUT:     " << inname    << "\n"
              << "OUTPUT   : " << outname   << "\n"
              << "MAX WORDS: " << max_words << "\n"
              ;

    if (inname.empty() || max_words <= 0) {
        std::cerr << "error: invalid arguments";
        return 1;
    }

    auto maybe_dict = load_dictionary(inname, max_words);
    if (!maybe_dict) {
        return 1;
    }
    const auto& dict = *maybe_dict;

    if (!test_dictionary(dict, inname, max_words)) {
        std::cerr << "dictionary test failed!" << std::endl;
        return 1;
    }

    return 0;
}
