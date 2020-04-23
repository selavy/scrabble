#include "mafsa.h"
#include <iostream>
#include <fstream>
#include <string_view>
#include <cassert>
#include <zlib.h>
#include <cstring>
#include <memory>
#include "mafsa_generated.h"

Mafsa::~Mafsa() noexcept
{
    mafsa_free(&mafsa_);
}

bool Mafsa::isword(const std::string& word) const noexcept
{
    return mafsa_isword(&mafsa_, word.c_str());
}

bool Mafsa::isterm(int s) const noexcept
{
    return mafsa_isterm(&mafsa_, s) != 0;
}

mafsa_edges Mafsa::get_edges(const char* const word) const noexcept
{
    return mafsa_prefix_edges(&mafsa_, word);
}

static bool ends_with(const std::string& s, std::string_view sv)
{
    return (
        s.size() >= sv.size() &&
        s.compare(s.size() - sv.size(), std::string::npos, sv) == 0
    );
}

static std::vector<char> read_gz_dict_file(const std::string& filename)
{
    gzFile file = gzopen(filename.c_str(), "rb");
    if (!file) {
        gzclose(file);
        throw std::runtime_error("unable to open input file");
    }
    constexpr std::size_t chunk = 1024;
    std::size_t len = 0;
    std::vector<char> buf(chunk, '\0');
    int rc;
    while ((rc = gzread(file, &buf[len], chunk)) > 0) {
        len += static_cast<std::size_t>(rc);
        buf.insert(buf.end(), chunk, '\0');
    }
    if (!gzeof(file)) {
        // TEMP TEMP
        int errnum = 0;
        std::cerr << "error: unable to read GZIP file [" << rc << "]: " << gzerror(file, &errnum) << std::endl;
        gzclose(file);
        throw std::runtime_error("unable to read input file -- not GZIP format?");
    }
    gzclose(file);
    assert(gzeof(file));
    using diff_type = typename std::vector<char>::difference_type;
    buf.erase(std::next(buf.cbegin(), static_cast<diff_type>(len)), buf.cend());
    return buf;
}


std::vector<char> read_dict_file(const std::string& filename)
{
    if (ends_with(filename, ".gz")) {
        return read_gz_dict_file(filename);
    }
    std::ifstream infile;
    infile.open(filename, std::ios::binary);
    infile.seekg(0, std::ios::end);
    std::streamoff length = infile.tellg();
    infile.seekg(0, std::ios::beg);
    std::vector<char> data(static_cast<typename std::vector<char>::size_type>(length));
    infile.read(data.data(), length);
    infile.close();
    return data;
}

std::optional<Mafsa> Mafsa::load(const std::string& filename)
{
    auto buf = read_dict_file(filename);
    auto serial_mafsa = GetSerialMafsa(buf.data());
    flatbuffers::Verifier v(reinterpret_cast<const uint8_t*>(buf.data()), buf.size());
    assert(serial_mafsa->Verify(v));

    // using uint = unsigned int;
    using Node = mafsa_node_;
    size_t size = serial_mafsa->nodes()->size();
    Node* nodes = reinterpret_cast<Node*>(malloc(size * sizeof(*nodes)));
    uint* terms = reinterpret_cast<uint*>(malloc(size * sizeof(*terms)));
    // auto nodes = std::make_unique<Node[]>(size);
    // auto terms = std::make_unique<uint[]>(size);
    memset(&nodes[0], 0, sizeof(nodes[0]) * size);
    memset(&terms[0], 0, sizeof(terms[0]) * size);
    size_t index = 0;
    for (const auto* node : *serial_mafsa->nodes()) {
        if (node->term()) {
            const auto entry = index / sizeof(terms[0]);
            const auto shift = index % sizeof(terms[0]);
            terms[entry] |= 1u << shift;
        }
        for (const auto* link : *node->children()) {
            nodes[index].children[link->value()] = link->next();
        }
        ++index;
    }
    Mafsa result;
    // result.mafsa_.nodes = nodes.release();
    // result.mafsa_.terms = terms.release();
    result.mafsa_.nodes = nodes;
    result.mafsa_.terms = terms;
    result.mafsa_.size  = static_cast<int>(size);
    return result;
}
