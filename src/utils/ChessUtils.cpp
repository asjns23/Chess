#include "utils/ChessUtils.hpp"

sf::Vector2i chessToBoard(char file, int rank)
{
    const int x = file - 'A';
    const int y = 8 - rank;

    return {x, y};
}

sf::Vector2f boardToPixel(sf::Vector2i boardPos, sf::Vector2f tileSize)
{
    const float x = boardPos.x * tileSize.x;
    const float y = boardPos.y * tileSize.y;

    return {x, y};
}