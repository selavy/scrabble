#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <variant>
#include <fstream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cicero/cicero.h>
#include <scrabble.h>
#include <mafsa++.h>

#define DEBUG(format, ...) fmt::print(std::cerr, "[DEBUG ({:s})]: " format "\n", __func__, ##__VA_ARGS__)
#define INFO(format, ...)  fmt::print(std::cerr, "[INFO  ({:s})]: " format "\n", __func__, ##__VA_ARGS__)

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// TODO: need ImVec4 to be constexpr
/*constexpr*/ ImVec4 MakeColor(float r, float g, float b, float a = 255) noexcept
{
    float sc = 1.0f/255.0f;
    float x = r * sc;
    float y = g * sc;
    float z = b * sc;
    float w = a * sc;
    return { x, y, z, w };
}

// clang-format off
/*constexpr*/ std::array<ImVec4, 3> TileTextColor = {
    MakeColor(  0,   0, 178), // blue -- Player 1
    MakeColor(178,   0,   0), // red  -- Player 2
    MakeColor(255, 255, 255), // white -- Normal
};
// TODO(peter): switch these to enums to index into array of images
/*constexpr*/ ImVec4 DoubleWordSquareColor   = MakeColor(250, 145, 145); // salmon
/*constexpr*/ ImVec4 TripleWordSquareColor   = MakeColor(215,   0,  75); // red
/*constexpr*/ ImVec4 DoubleLetterSquareColor = MakeColor( 50,  50, 233); // dark blue
/*constexpr*/ ImVec4 TripleLetterSquareColor = MakeColor(150, 200, 250); // light blue
/*constexpr*/ ImVec4 EmptySquareColor        = MakeColor( 66,  66,  66); // gray
/*constexpr*/ ImVec4 TileSquareColor         = MakeColor(120,  81,  45); // oak
/*constexpr*/ ImVec4 TileHoverSquareColor    = MakeColor(206, 187, 158); // light oak
ImVec4 square_colors[225] = {
    TripleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor,
    EmptySquareColor       , EmptySquareColor       , EmptySquareColor       , TripleWordSquareColor  ,
    EmptySquareColor       , EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor,
    EmptySquareColor       , EmptySquareColor       , TripleWordSquareColor  , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , DoubleWordSquareColor  , EmptySquareColor       ,
    EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor,
    EmptySquareColor       , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , DoubleWordSquareColor  , EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       ,
    EmptySquareColor       , TripleWordSquareColor  , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , TripleWordSquareColor  ,
    EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , DoubleWordSquareColor  , EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor,
    EmptySquareColor       , EmptySquareColor       , DoubleWordSquareColor  , EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       ,
    DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    TripleLetterSquareColor, EmptySquareColor       , EmptySquareColor       , EmptySquareColor       ,
    DoubleWordSquareColor  , EmptySquareColor       , TripleWordSquareColor  , EmptySquareColor       ,
    EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       , EmptySquareColor       ,
    EmptySquareColor       , TripleWordSquareColor  , EmptySquareColor       , EmptySquareColor       ,
    EmptySquareColor       , EmptySquareColor       , DoubleLetterSquareColor, EmptySquareColor       ,
    TripleWordSquareColor  ,
};
// clang-format on

struct Callbacks
{
    using Move = scrabble::Move;

    Callbacks(Mafsa&& m) noexcept : mafsa_{std::move(m)}, legal_moves_{} {}

    static cicero_edges prefix_edges(void* data, const char* prefix) noexcept
    {
        auto* self = reinterpret_cast<const Callbacks*>(data);
        return self->prefix_edges_(prefix);
    }

    static void on_legal_move(void* data, const char* word, int square, int direction) noexcept
    {
        auto* self = reinterpret_cast<Callbacks*>(data);
        return self->on_legal_move_(word, square, direction);
    }

    void clear_legal_moves() noexcept
    {
        legal_moves_.clear();
    }

    std::vector<Move> sorted_legal_moves() const noexcept
    {
        auto result = legal_moves_;
        std::sort(result.begin(), result.end());
        return result;
    }

    cicero_callbacks make_callbacks() const noexcept
    {
        cicero_callbacks cb;
        cb.onlegal = &Callbacks::on_legal_move;
        cb.getedges = &Callbacks::prefix_edges;
        cb.onlegaldata = this;
        cb.getedgesdata = this;
        return cb;
    }

    bool isword(const std::string& word) const noexcept
    {
        return mafsa_.isword(word);
    }

private:
    cicero_edges prefix_edges_(const char* prefix) const noexcept
    {
        cicero_edges out;
        auto edges = mafsa_.get_edges(prefix);
        static_assert(sizeof(edges) == sizeof(out));
        memcpy(&out, &edges, sizeof(out));
        return out;
    }

    void on_legal_move_(const char* word, int square, int direction)
    {
        legal_moves_.emplace_back();
        legal_moves_.back().square = scrabble::Square{square};
        legal_moves_.back().direction = static_cast<scrabble::Direction>(direction);
        legal_moves_.back().word = word;
        legal_moves_.back().score = -1;
        DEBUG("Found legal move: {}", legal_moves_.back());
    }

    Mafsa             mafsa_;
    std::vector<Move> legal_moves_;
};

template <class T>
auto get_dnd_payload(const ImGuiPayload* payload) -> const T*
{
    IM_ASSERT(payload->DataSize == sizeof(T));
    return static_cast<const T*>(payload->Data);
}

struct TileBankToRackAction
{
    int newtile;
    int oldtile;
    int rackidx;
};

struct RackTileToBoardAction
{
    int oldtile;
    int newtile;
    int rackidx;
    int sq;
};

struct ClearBoardTileAction
{
    int tile;
    int sq;
};

struct ClearRackTileAction
{
    int tile;
    int rackidx;
};

using Action = std::variant<
      TileBankToRackAction
    , RackTileToBoardAction
    , ClearBoardTileAction
    , ClearRackTileAction
>;

inline std::ostream& operator<<(std::ostream& os, const cicero& engine)
{
    auto print_row = [&os](const cicero& e, int row) {
        for (int col = 0; col < Dim - 1; ++col) {
            os << cicero_tile_on_square(&e, row*Dim + col) << " | ";
        }
        os << cicero_tile_on_square(&e, row*Dim + (Dim - 1));
    };
    const auto* b = engine.vals;
    os << "     1   2   3   4   5   6   7   8   9   0   1   2   3   4   5  \n";
    os << "   -------------------------------------------------------------\n";
    for (int row = 0; row < Dim; ++row) {
        os << static_cast<char>('A' + row) << "  | ";
        print_row(engine, row);
        os << " |\n";
        os << "   -------------------------------------------------------------\n";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const cicero_rack& rack)
{
    os << "\"";
    int num_tiles = 0;
    for (int i = 0; i < 26; ++i) {
        for (int j = 0; j < rack.tiles[i]; ++j) {
            os << static_cast<char>(i + 'A');
            ++num_tiles;
        }
    }
    for (int i = 0; i < rack.tiles[26]; ++i) {
        os << "?";
        ++num_tiles;
    }
    while (num_tiles++ < 7) {
        os << " ";
    }
    os << "\"";
    return os;
}

int main(int argc, char** argv)
{
    // TODO: add argument parser
    // auto dictfile = "csw19.dict.gz";
    auto dictfile = "enable1.dict.gz";
    auto maybe_dict = Mafsa::load(dictfile);
    if (!maybe_dict) {
        fmt::print(std::cerr, "Unable to load dictionary from file: '{}'\n", dictfile);
        return 1;
    }
    auto cb = Callbacks{std::move(*maybe_dict)};
    cicero engine;

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    if (gl3wInit() != 0) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.FontGlobalScale = 1.5;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use
    // ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application
    // (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double
    // backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL,
    // io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = false;
    bool show_metrics_window = true;
    bool show_scrabble_window = true;

    const char* square_text = "";

    constexpr std::array<const char*, 15> column_labels = {
        " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11",
        "12", "13", "14", "15",
    };
    constexpr std::array<const char*, 15> row_labels = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    };

    constexpr std::array<const char*, 55> tile_labels = {
        // regular tiles
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
        // blanks on board
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
        "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
        // special tiles
        "?", // blank tile in rack
        " ",  // empty tile
        "@"  // <<< signifies an error
    };
    constexpr int ErrorTileNum = 54;
    constexpr int EmptyTileNum = ErrorTileNum - 1;
    constexpr int BlankTileNum = EmptyTileNum - 1;
    static_assert(ErrorTileNum == tile_labels.size() - 1);

    auto tile_label_to_tile_number = [&](const char* label) -> int
    {
        auto found = std::find(std::begin(tile_labels), std::end(tile_labels), label);
        if (found == std::end(tile_labels)) {
            return ErrorTileNum;
        }
        return static_cast<int>(std::distance(std::begin(tile_labels), found));
    };

    std::array<int, 225> board_labels;
    std::fill(std::begin(board_labels), std::end(board_labels), EmptyTileNum);

    std::array<int, 7> rack;
    std::fill(std::begin(rack), std::end(rack), EmptyTileNum);

    if (argc >= 2) {
        const char* filename = argv[1];
        auto res = scrabble::read_board(filename);
        if (res) {
            auto&& [board_, rack_] = *res;
            for (std::size_t i = 0; i < rack_.size() && i < rack.size(); ++i) {
                auto ch = rack_[i];
                rack[i] = ch == ' ' ? EmptyTileNum : ch - 'A';
            }

            for (std::size_t i = 0; i < board_.size(); ++i) {
                auto ch = board_[i];
                if (ch == ' ') {
                    board_labels[i] = EmptyTileNum;
                } else if ('A' <= ch && ch <= 'Z') {
                    board_labels[i] = ch - 'A';
                } else if ('a' <= ch && ch <= 'z') {
                    board_labels[i] = (ch - 'a') + 26;
                } else {
                    DEBUG("Invalid tile: {}", ch);
                }
            }

            DEBUG("Position loaded from file {}!", filename);
        }

    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your
        // inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those
        // two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        if (show_metrics_window) {
            ImGui::ShowMetricsWindow(&show_metrics_window);
        }

        if (ImGui::Begin("Scrabble", &show_scrabble_window, ImGuiWindowFlags_MenuBar)) {
            ImGui::BeginGroup();

            { // Column Labels
                ImGui::BeginGroup();
                ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                ImGui::Button("  ", ImVec2(40, 40));
                for (int col = 0; col < 15; ++col) {
                    ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                    ImGui::Button(column_labels[col], ImVec2(40, 40));
                }
                ImGui::EndGroup();
                ImGui::NewLine();
            }

            int id = 0;
            int index = 0;
            for (int row = 0; row < 15; ++row) {
                ImGui::BeginGroup();

                { // Row Labels
                    ImGui::PushID(id++);
                    ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                    ImGui::Button(row_labels[row], ImVec2(40, 40));
                    ImGui::PopID();
                }

                for (int col = 0; col < 15; ++col, ++index) {
                    ImGui::PushID(id++);
                    auto square_color = square_colors[index];
                    ImGui::PushStyleColor(ImGuiCol_Button, square_color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, square_color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, square_color);
                    ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                    if (ImGui::Button(tile_labels[board_labels[index]], ImVec2(40, 40))) {
                        DEBUG("board tile was clicked.");
                        // reset tile on left click
                        board_labels[index] = EmptyTileNum;
                    }
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TILE")) {
                            DEBUG("Accepting tile bank tile");
                            const auto tile = *get_dnd_payload<int>(payload);
                            assert(0 <= tile && tile < tile_labels.size());
                            board_labels[index] = tile;

                            if (tile == BlankTileNum) {
                                DEBUG("Blank tile begin placed on board");
                                ImGui::OpenPopup("BLANK_TILE_SELECTOR", ImGuiPopupFlags_AnyPopup | ImGuiPopupFlags_NoOpenOverExistingPopup);
                            }
                        }
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RACK_TILE")) {
                            DEBUG("Accepting rack tile");
                            const auto rack_index = *get_dnd_payload<int>(payload);
                            assert(0 <= rack_index && rack_index < rack.size());
                            board_labels[index] = rack[rack_index];
                            rack[rack_index] = EmptyTileNum;
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                }
                ImGui::EndGroup();
                ImGui::NewLine();
            }
            ImGui::EndGroup();
            ImGui::NewLine();

            ///////////////////////////////////////////////////////////////////
            // TODO:
            // Trying to figure out how to make a pop up selector for blank
            // tiles. For some reason, the pop-up *does* work if I drive it
            // with the below button, but it *does not* work if I do it in the
            // DND target code above.
            // ////////////////////////////////////////////////////////////////

            if (ImGui::Button("Select...")) {
                ImGui::OpenPopup("BLANK_TILE_SELECTOR");
            }
            ImGui::NewLine();

            if (ImGui::BeginPopup("BLANK_TILE_SELECTOR")) {
                DEBUG("BeginPopup returned true");
                for (int tile = 26; tile < 2*26; ++tile) {
                    ImGui::PushID(id++);
                    ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                    ImGui::Button(tile_labels[tile], ImVec2(40, 40));
                    ImGui::PopID();
                    if (tile % 5 == 0) {
                        ImGui::NewLine();
                    }
                }
                ImGui::NewLine();
                ImGui::EndPopup();
            }

            { // rack
                ImGui::BeginGroup();
                for (int index = 0; index < rack.size(); ++index) {
                    const auto tile = rack[index];
                    ImGui::PushID(id++);
                    ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                    if (ImGui::Button(tile_labels[tile], ImVec2(40, 40))) {
                        // reset tile on left click
                        rack[index] = EmptyTileNum;
                    }
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        ImGui::SetDragDropPayload("DND_RACK_TILE", &index, sizeof(index));
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginDragDropTarget()) {
                        if (const auto* payload = ImGui::AcceptDragDropPayload("DND_TILE")) {
                            rack[index] = *get_dnd_payload<int>(payload);
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::PopID();
                }
                ImGui::EndGroup();
                ImGui::NewLine();
            }

            // tile bank
            ImGui::BeginGroup();
            for (int tile = 0; tile < 2*26 + 1; ++tile) {
                if (tile % 13 == 0) {
                    ImGui::NewLine();
                }
                ImGui::PushID(id++);
                ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                ImGui::Button(tile_labels[tile], ImVec2(40, 40));
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("DND_TILE", &tile, sizeof(tile));
                    ImGui::EndDragDropSource();
                }
                ImGui::PopID();
            }
            ImGui::EndGroup();
            ImGui::NewLine();

            ImGui::BeginGroup();
            if (ImGui::Button("Find Best Moves")) {
                DEBUG("FindBestMoves button pressed");
                cicero_init_wwf(&engine, cb.make_callbacks());
                cicero_rack erack;
                memset(&erack, 0, sizeof(erack));
                for (auto tilenum : rack) {
                    if (tilenum == EmptyTileNum) {
                        continue;
                    }
                    if ((0 <= tilenum && tilenum < 26) || tilenum == BlankTileNum) {
                        char tile = tile_labels[tilenum][0];
                        DEBUG("Adding tile {} to rack", tile);
                        cicero_rack_add_tile(&erack, tile);
                    }
                }

                DEBUG("Generating board");
                char board[225];
                for (std::size_t i = 0; i < board_labels.size(); ++i) {
                    board[i] = tile_labels[board_labels[i]][0];
                }

                cicero_load_position(&engine, board);
                DEBUG("Board:\n{}\nRack: {}", engine, erack);

                DEBUG("Generating legal moves...");
                cb.clear_legal_moves();
                cicero_generate_legal_moves(&engine, erack);
                DEBUG("Finished generating legal moves...");
                auto moves = cb.sorted_legal_moves();
                cicero_savepos sp;
                for (auto& move : moves) {
                    auto emove = scrabble::EngineMove::make(&engine, move);
                    auto cmove = emove.move;
                    move.score = cicero_make_move(&engine, &sp, &cmove);
                    cicero_undo_move(&engine, &sp, &cmove);
                }
                std::sort(moves.begin(), moves.end(),
                        [](const auto& m1, const auto& m2) {
                            return m1.score > m2.score;
                        });
                auto N = std::min<std::size_t>(moves.size(), 20u);
                for (std::size_t i = 0; i < N; ++i) {
                    INFO("{}", moves[i]);
                }
            }
            ImGui::EndGroup();
            ImGui::NewLine();
        }
        ImGui::End();


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
