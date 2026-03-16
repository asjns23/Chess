#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Config.hpp"

class Board : public sf::Drawable
{
public:
    Board(sf::Vector2f origin,
          const Config::Board& config,
          const sf::Texture& boardSquaresTexture,
          sf::Color lightColor,
          sf::Color darkColor);

    bool isInside(sf::Vector2i square) const;
    sf::Vector2f squareToPixel(sf::Vector2i square) const;
    sf::Vector2i pixelToSquare(sf::Vector2i pixel) const;

    float getWidth() const;
    float getHeight() const;

    // Colors extracted from board_squares sprite for light and dark squares
    sf::Color getLightColor() const;
    sf::Color getDarkColor() const;

private:
    void buildSprites();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Vector2f m_origin;

    int m_columns;
    int m_rows;
    float m_squareSize;
    float m_border;

    const sf::Texture* m_boardSquaresTexture;

    // Colors and texture references for light and dark squares
    sf::Color m_lightColor;
    sf::Color m_darkColor;
    sf::IntRect m_lightRect;
    sf::IntRect m_darkRect;

    sf::RectangleShape m_borderShape;
    std::vector<sf::Sprite> m_squareSprites;
};