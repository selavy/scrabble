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

// light gray =  regular
// green = triple letter
// blue = double letter
// red = double word
// orange = triple word

#define DEBUG(format, ...) fmt::print(std::cerr, "[DEBUG ({:s})]: " format "\n", __func__, ##__VA_ARGS__)
#define INFO(format, ...)  fmt::print(std::cerr, "[INFO  ({:s})]: " format "\n", __func__, ##__VA_ARGS__)

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

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

ImVec4 MakeColor(float r, float g, float b, float a = 255) noexcept
{
    float sc = 1.0f/255.0f;
    float x = r * sc;
    float y = g * sc;
    float z = b * sc;
    float w = a * sc;
    return { x, y, z, w };
}

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------

// clang-format off
const ImVec4 TextColor_Black         = MakeColor(  0,   0,   0); // black
const ImVec4 TextColor_White         = MakeColor(255, 255, 255); // white
const ImVec4 DoubleWordSquareColor   = MakeColor(250, 145, 145); // salmon
const ImVec4 TripleWordSquareColor   = MakeColor(215,   0,  75); // red
const ImVec4 DoubleLetterSquareColor = MakeColor( 50,  50, 233); // dark blue
const ImVec4 TripleLetterSquareColor = MakeColor(150, 200, 250); // light blue
const ImVec4 EmptySquareColor        = MakeColor( 66,  66,  66); // gray
const ImVec4 TileSquareColor         = MakeColor(120,  81,  45); // oak
const ImVec4 TileHoverSquareColor    = MakeColor(206, 187, 158); // light oak
const ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// clang-format on

constexpr std::array<const char*, 16> ColumnLabels = {
    "  ", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15",
};

constexpr std::array<const char*, 15> RowLabels = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
};

constexpr std::array<const char*, 55> TileLabels = {
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

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
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

    GLFWwindow* window = glfwCreateWindow(1280, 720, "CiceroUI", NULL, NULL);
    if (window == NULL) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

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
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // State
    bool show_metrics_window = true;
    bool show_cicero_window = true;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            if (show_metrics_window) {
                ImGui::ShowMetricsWindow(&show_metrics_window);
            }

            if (ImGui::Begin("Cicero", &show_cicero_window, ImGuiWindowFlags_MenuBar))
            {
                ImGui::BeginGroup();

                int id = 0;
                // TODO: choose border tile + text colors
                const auto border_color = DoubleWordSquareColor;
                const auto border_text_color = TextColor_Black;
                const auto cell_spacing = 5.;

                { // Column Labels
                    ImGui::BeginGroup();
                    for (auto column_label : ColumnLabels) {
                        ImGui::SameLine(0., cell_spacing);
                        ImGui::PushID(id++);
                        ImGui::PushStyleColor(ImGuiCol_Text, border_text_color);
                        ImGui::PushStyleColor(ImGuiCol_Button, border_color);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, border_color);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, border_color);
                        ImGui::Button(column_label, ImVec2(40, 40));
                        ImGui::PopStyleColor(4);
                        ImGui::PopID();
                    }
                    ImGui::EndGroup();
                    ImGui::NewLine();
                }

                // int square_index = 0;
                // for (int row = 0; row < 15; ++row) {
                //     ImGui::PushID(id++);
                // }

                ImGui::EndGroup();
            }
            ImGui::End();
        }
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
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
