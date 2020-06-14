#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <array>
#include <string>
#include <vector>
#include <algorithm>

#include <fmt/format.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

int main(int argc, char** argv)
{
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

    std::array<const char*, 15> column_labels = {
        " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11",
        "12", "13", "14", "15",
    };
    std::array<const char*, 15> row_labels = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    };

    std::array<const char*, 27> tile_labels = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "?",
    };

    const char* empty_square_label = "";
    std::array<const char*, 225> board_labels;
    std::fill(std::begin(board_labels), std::end(board_labels), empty_square_label);

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

        ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::ShowMetricsWindow(&show_metrics_window);

        ImGui::Begin("Scrabble", &show_scrabble_window, ImGuiWindowFlags_MenuBar);

        ImGui::BeginGroup();

        {
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

        int index = 0;
        for (int row = 0; row < 15; ++row) {
            ImGui::BeginGroup();

            {
                // ImGui::PushID(index);
                ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                ImGui::Button(row_labels[row], ImVec2(40, 40));
                // ImGui::PopID();
            }

            for (int col = 0; col < 15; ++col, ++index) {
                auto square_color = square_colors[index];
                ImGui::PushID(index);
                // ImGui::PushStyleColor(ImGuiCol_Text, TileTextColor[tile_color]);
                ImGui::PushStyleColor(ImGuiCol_Button, square_color);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, square_color);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, square_color);
                ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
                if (ImGui::Button(board_labels[index], ImVec2(40, 40))) {
                    // button was clicked -- reset tile
                    board_labels[index] = empty_square_label;
                }
                ImGui::PopStyleColor(/*4*/3);
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TILE")) {
                        IM_ASSERT(payload->DataSize == sizeof(index));
                        int tile = *static_cast<const int*>(payload->Data);
                        assert(0 <= tile && tile <= 27);
                        board_labels[index] = tile_labels[tile];
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }
            ImGui::EndGroup();
            ImGui::NewLine();
        }
        ImGui::EndGroup();
        ImGui::NewLine();

        // tile bank
        ImGui::BeginGroup();
        for (int tile = 0; tile < 27; ++tile) {
            if (tile % 13 == 0) {
                ImGui::NewLine();
            }
            ImGui::SameLine(/*offset_from_start_x*/0., /*spacing*/5.);
            ImGui::Button(tile_labels[tile], ImVec2(40, 40));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("DND_TILE", &tile, sizeof(tile));
                ImGui::EndDragDropSource();
            }
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
