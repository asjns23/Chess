#pragma once

#include <SFML/Graphics.hpp>

enum class Color
{
    White,
    Black
};

enum class PieceType
{
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

class ChessPiece
{
public:
    ChessPiece(Color c, PieceType type, sf::Vector2i pos, const sf::Texture& t);
    virtual ~ChessPiece() = default;

    virtual void move(sf::Vector2i newPos);
    virtual void render(sf::RenderWindow& window) const;

    Color getColor() const;
    PieceType getType() const;
    sf::Vector2i getPosition() const;

    void setPixelPosition(sf::Vector2f pixelPos);
    void setScale(sf::Vector2f scale);

protected:
    Color m_color;
    PieceType m_type;
    sf::Vector2i m_position;
    sf::Sprite m_sprite;
};
