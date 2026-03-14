#include "pieces/ChessPiece.hpp"

ChessPiece::ChessPiece(Color c, PieceType type, sf::Vector2i pos, const sf::Texture& t)
    : m_color(c),
      m_type(type),
      m_position(pos),
      m_sprite(t)
{
}

void ChessPiece::move(sf::Vector2i newPos)
{
    m_position = newPos;
}

void ChessPiece::render(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}

Color ChessPiece::getColor() const
{
    return m_color;
}

PieceType ChessPiece::getType() const
{
    return m_type;
}

sf::Vector2i ChessPiece::getPosition() const
{
    return m_position;
}

void ChessPiece::setPixelPosition(sf::Vector2f pixelPos)
{
    m_sprite.setPosition(pixelPos);
}

void ChessPiece::setScale(sf::Vector2f scale)
{
    m_sprite.setScale(scale);
}
