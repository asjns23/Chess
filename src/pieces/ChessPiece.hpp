#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <optional>

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

enum class MoveDir
{
    Up,
    Down,
    Left,
    Right
};

struct QueuedInput
{
    MoveDir dir;
    float timeSeconds;
};

class ChessPiece
{
public:
    ChessPiece(Color c, PieceType type, sf::Vector2i pos, const sf::Texture& t);
    virtual ~ChessPiece() = default;

    virtual void resetInputState();

    virtual void move(sf::Vector2i newPos);
    virtual std::optional<sf::Vector2i> nextMove(std::deque<QueuedInput>& queue) const = 0;
    virtual bool isValidMove(sf::Vector2i newPos) const = 0;
    virtual void render(sf::RenderWindow& window) const;

    Color getColor() const;
    PieceType getType() const;
    sf::Vector2i getPosition() const;

    void setPixelPosition(sf::Vector2f pixelPos);
    void setScale(sf::Vector2f scale);

    static std::optional<MoveDir> keyToDir(sf::Keyboard::Key key);

protected:
    Color m_color;
    PieceType m_type;
    sf::Vector2i m_position;
    sf::Sprite m_sprite;
};

class Knight final : public ChessPiece
{
public:
    static constexpr PieceType type = PieceType::Knight;

    Knight(Color c, sf::Vector2i pos, const sf::Texture& t);

    bool isValidMove(sf::Vector2i newPos) const override;
    std::optional<sf::Vector2i> nextMove(std::deque<QueuedInput>& queue) const override;
};

class King final : public ChessPiece
{
public:
    static constexpr PieceType type = PieceType::King;

    King(Color c, sf::Vector2i pos, const sf::Texture& t);

    bool isValidMove(sf::Vector2i newPos) const override;
    std::optional<sf::Vector2i> nextMove(std::deque<QueuedInput>& queue) const override;
};