#include "Board.hpp"

Board::Board(sf::Vector2f origin, float boardPixelSize)
    : m_origin(origin),
      m_boardPixelSize(boardPixelSize),
      m_squareSize(boardPixelSize / 8.0f)
{
}

bool Board::isInside(sf::Vector2i square) const
{
    return square.x >= 0 && square.x < 8 &&
           square.y >= 0 && square.y < 8;
}

sf::Vector2f Board::squareToPixel(sf::Vector2i square) const
{
    return {
        m_origin.x + static_cast<float>(square.x) * m_squareSize,
        m_origin.y + static_cast<float>(square.y) * m_squareSize
    };
}

sf::Vector2i Board::pixelToSquare(sf::Vector2i pixel) const
{
    return {
        static_cast<int>((pixel.x - m_origin.x) / m_squareSize),
        static_cast<int>((pixel.y - m_origin.y) / m_squareSize)
    };
}