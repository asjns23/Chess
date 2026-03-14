#pragma once

#include <SFML/Graphics.hpp>

class Board
{
public:
    Board(sf::Vector2f origin, float boardPixelSize);

    bool isInside(sf::Vector2i square) const;
    sf::Vector2f squareToPixel(sf::Vector2i square) const;
    sf::Vector2i pixelToSquare(sf::Vector2i pixel) const;

private:
    sf::Vector2f m_origin;
    float m_boardPixelSize;
    float m_squareSize;
};
