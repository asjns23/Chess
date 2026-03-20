#pragma once

#include <string_view>

#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

#ifndef ASSET_DIR
#define ASSET_DIR "assets"
#endif

namespace Config
{
    namespace Paths
    {
        inline constexpr std::string_view ProjectRoot = PROJECT_ROOT;
        inline constexpr std::string_view AssetDir = ASSET_DIR;
        inline constexpr std::string_view BoardDir = ASSET_DIR "/board";
        inline constexpr std::string_view PiecesDir = ASSET_DIR "/pieces";
        inline constexpr std::string_view BoardSquaresFile = "board_squares.png";
        inline constexpr std::string_view BackgroundFile = "bg.png";
    }

    struct Window
    {
        unsigned int width = 1280;
        unsigned int height = 720;
        const char* title = "Chess";
    };

    struct Board
    {
        int columns = 6;
        int rows = 6;
        float squareSize = 88.0f;
        float border = 4.0f;
    };

    inline const Window window{};
    inline const Board board{};
}