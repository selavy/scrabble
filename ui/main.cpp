#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <array>
#include <fmt/format.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

constexpr int NumRows = 15;
constexpr int NumCols = 15;
constexpr int NumSquares = NumRows * NumCols;
constexpr int NumRackTiles = 7;

constexpr int ix(int col, int row) noexcept
{
    return col*NumRows + row;
}

constexpr ImVec4 MakeColor(float r, float g, float b, float a = 255) noexcept
{
    float sc = 1.0f/255.0f;
    float x = r * sc;
    float y = g * sc;
    float z = b * sc;
    float w = a * sc;
    return { x, y, z, w };
}

// clang-format off
constexpr std::array<ImVec4, 3> TileTextColor = {
    MakeColor(  0,   0, 178), // blue -- Player 1
    MakeColor(178,   0,   0), // red  -- Player 2
    MakeColor(255, 255, 255), // white -- Normal
};
// TODO(peter): switch these to enums to index into array of images
constexpr ImVec4 DoubleWordSquareColor   = MakeColor(250, 145, 145); // salmon
constexpr ImVec4 TripleWordSquareColor   = MakeColor(215,   0,  75); // red
constexpr ImVec4 DoubleLetterSquareColor = MakeColor( 50,  50, 233); // dark blue
constexpr ImVec4 TripleLetterSquareColor = MakeColor(150, 200, 250); // light blue
constexpr ImVec4 EmptySquareColor        = MakeColor( 66,  66,  66); // gray
constexpr ImVec4 TileSquareColor         = MakeColor(120,  81,  45); // oak
constexpr ImVec4 TileHoverSquareColor    = MakeColor(206, 187, 158); // light oak
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

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

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
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (gl3wInit() != 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.FontGlobalScale = 1.5;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = false;
    bool show_metrics_window = true;
    bool show_scrabble_window = true;

    // TODO: really only needs to be char[2]
    std::vector<std::string> tile_labels;
    for (int c = 0; c < 256; ++c) {
        if (('A' <= c && c <= 'Z') || (c == '?') || (c == ' ')) {
            tile_labels.emplace_back(fmt::format("{:c}", static_cast<char>(c)));
        } else {
            tile_labels.emplace_back("");
        }
    }
    auto TileLabel = [&tile_labels](int i) -> const char* {
        assert(0 <= i && i < tile_labels.size());
        return tile_labels[i].c_str();
    };
    constexpr size_t BlankTileIndex = static_cast<size_t>('?');
    constexpr size_t EmptyTileIndex = static_cast<size_t>(' ');
    std::array<int, NumRackTiles> player1_tiles = {
        'I', 'E', 'G', 'U', 'J', 'A', 'X',
    };
    std::array<int, NumRackTiles> player2_tiles = {
        'T', 'N', 'E', 'M', 'W', 'O', 'L',
    };

    std::vector<const char*> board_labels(NumSquares, TileLabel(EmptyTileIndex));
    // std::fill(std::begin(board_labels), std::end(board_labels), TileLabel(EmptyTileIndex));
    std::array<const char*, NumRackTiles> player1_rack_labels;
    std::array<const char*, NumRackTiles> player2_rack_labels;
    for (size_t i = 0; i < NumRackTiles; ++i) {
        player1_rack_labels[i] = TileLabel(player1_tiles[i]);
        player2_rack_labels[i] = TileLabel(player2_tiles[i]);
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::ShowMetricsWindow(&show_metrics_window);

        ImGui::Begin("Scrabble1", &show_scrabble_window, ImGuiWindowFlags_MenuBar);

        // Board
        int index;

        index = 0;
        ImGui::BeginGroup();
        for (int col = 0; col < NumCols; ++col) {
            ImGui::BeginGroup();
            for (int row = 0; row < NumRows; ++row) {
                auto color = square_colors[index];
                auto tile_color = 2;
                if (ix(col, row) == 32) {
                    tile_color = 0;
                } else if (ix(col, row) == 24) {
                    tile_color = 1;
                }
                const auto* text = board_labels[index];

                ImGui::PushID(index);
                ImGui::PushStyleColor(ImGuiCol_Text, TileTextColor[tile_color]);
                ImGui::PushStyleColor(ImGuiCol_Button, color);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
                ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                ImGui::Button(text, ImVec2(40, 40));
                ImGui::PopStyleColor(4);

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TILE")) {
                        IM_ASSERT(payload->DataSize == sizeof(index));
                        int payload_index = *static_cast<const int*>(payload->Data);
                        assert(0 <= payload_index && payload_index < 2*NumRackTiles);
                        if (payload_index < NumRackTiles) {
                            int rack_index = payload_index;
                            assert(0 <= rack_index && rack_index < player1_tiles.size());
                            board_labels[index] = TileLabel(player1_tiles[rack_index]);
                            player1_tiles[rack_index] = EmptyTileIndex;
                        } else {
                            int rack_index = payload_index - NumRackTiles;
                            assert(0 <= rack_index && rack_index < player2_tiles.size());
                            board_labels[index] = TileLabel(player2_tiles[rack_index]);
                            player2_tiles[rack_index] = EmptyTileIndex;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::PopID();
                ++index;
            }
            ImGui::EndGroup();
            ImGui::NewLine();
        }
        ImGui::EndGroup();

        index = 0;
        ImGui::BeginGroup();
        for (int tile : player1_tiles) {
            player1_rack_labels[index] = TileLabel(tile);
            ImGui::PushID(NumSquares + index);
            ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
            ImGui::PushStyleColor(ImGuiCol_Button, TileSquareColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, TileHoverSquareColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, TileHoverSquareColor);
            ImGui::Button(player1_rack_labels[index], ImVec2(40, 40));
            ImGui::PopStyleColor(3);
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("DND_TILE", &index, sizeof(index));
                ImGui::EndDragDropSource();
            }
            ImGui::PopID();
            ++index;
        }
        ImGui::EndGroup();
        ImGui::NewLine();

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
