#include "utils/ChessUtils.hpp"
#include "Config.hpp"

#include <cctype>
#include <string>
#include <string_view>

namespace
{
    int normalizeFile(char file)
    {
        const char upper = static_cast<char>(
            std::toupper(static_cast<unsigned char>(file))
        );

        if (upper < 'A' || upper >= 'A' + Config::board.columns)
        {
            return -1;
        }

        return (upper - 'A') + 1; // A=1, B=2, ...
    }

    sf::Vector2i invalidSquare()
    {
        return {-1, -1};
    }
}

// Converts chess notation (e.g., 'A', 1) to board coordinates (0-based)
sf::Vector2i chessToBoard(int file, int rank)
{
    if (file < 1 || file > Config::board.columns)
    {
        return invalidSquare();
    }

    if (rank < 1 || rank > Config::board.rows)
    {
        return invalidSquare();
    }

    const int x = file - 1;
    const int y = Config::board.rows - rank;

    return {x, y};
}

// Overload for character file (e.g., 'A', 1)
sf::Vector2i chessToBoard(char file, int rank)
{
    const int normalizedFile = normalizeFile(file);
    if (normalizedFile == -1)
    {
        return invalidSquare();
    }

    return chessToBoard(normalizedFile, rank);
}

// Overload for chess notation string (e.g., "A1")
sf::Vector2i chessToBoard(std::string_view square)
{
    if (square.size() < 2)
    {
        return invalidSquare();
    }

    const int file = normalizeFile(square[0]);
    if (file == -1)
    {
        return invalidSquare();
    }

    int rank = 0;
    for (std::size_t i = 1; i < square.size(); ++i)
    {
        const unsigned char ch = static_cast<unsigned char>(square[i]);
        if (!std::isdigit(ch))
        {
            return invalidSquare();
        }

        rank = rank * 10 + (square[i] - '0');
    }

    return chessToBoard(file, rank);
}

// Overload for C-string chess notation (e.g., "A1")
sf::Vector2i chessToBoard(const char* square)
{
    if (square == nullptr)
    {
        return invalidSquare();
    }

    return chessToBoard(std::string_view(square));
}

// Converts board coordinates to pixel coordinates (top-left corner of the square)
sf::Vector2f boardToPixel(sf::Vector2i boardPos, float squareSize)
{
    return {
        static_cast<float>(boardPos.x) * squareSize,
        static_cast<float>(boardPos.y) * squareSize
    };
}