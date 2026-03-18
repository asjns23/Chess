#include "pieces/ChessPiece.hpp"

ChessPiece::ChessPiece(Color c, PieceType type, sf::Vector2i pos, const sf::Texture& t)
    : m_color(c),
      m_type(type),
      m_position(pos),
      m_sprite(t)
{
}

std::optional<MoveDir> ChessPiece::keyToDir(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::Up:    return MoveDir::Up;

        case sf::Keyboard::Key::S:
        case sf::Keyboard::Key::Down:  return MoveDir::Down;

        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::Left:  return MoveDir::Left;

        case sf::Keyboard::Key::D:
        case sf::Keyboard::Key::Right: return MoveDir::Right;

        default:
            return std::nullopt;
    }
}

std::optional<sf::Vector2i> ChessPiece::handleDirectionalInput(sf::Keyboard::Key)
{
    return std::nullopt;
}

void ChessPiece::resetInputState()
{
    // default: nothing
}

void ChessPiece::move(sf::Vector2i newPos)
{
    m_position = newPos;
}

bool ChessPiece::isValidMove(sf::Vector2i newPos) const
{
    // This function should be overridden to implement specific move logic for each piece type.
    return false;
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
