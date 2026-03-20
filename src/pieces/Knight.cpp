#include "pieces/ChessPiece.hpp"

#include <cstdlib>

Knight::Knight(Color c, sf::Vector2i pos, const sf::Texture& t)
    : ChessPiece(c, PieceType::Knight, pos, t)
{
}

bool Knight::isValidMove(sf::Vector2i newPos) const
{
    const sf::Vector2i delta = newPos - m_position;
    const int dx = std::abs(delta.x);
    const int dy = std::abs(delta.y);

    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

std::optional<sf::Vector2i> Knight::nextMove(std::deque<QueuedInput>& queue) const
{
    constexpr float sequenceWindow = 0.20f;

    if (queue.size() < 2)
    {
        return std::nullopt;
    }

    const QueuedInput first = queue[0];
    const QueuedInput second = queue[1];

    const float dt = second.timeSeconds - first.timeSeconds;
    if (dt > sequenceWindow)
    {
        queue.pop_front();
        return std::nullopt;
    }

    const bool firstHorizontal =
        first.dir == MoveDir::Left || first.dir == MoveDir::Right;

    const bool secondHorizontal =
        second.dir == MoveDir::Left || second.dir == MoveDir::Right;

    // Knight sequenced input must be one horizontal and one vertical input.
    if (firstHorizontal == secondHorizontal)
    {
        queue.pop_front();
        return std::nullopt;
    }

    queue.pop_front();
    queue.pop_front();

    int dx = 0;
    int dy = 0;

    auto applyLeg = [](MoveDir dir, int amount, int& x, int& y)
    {
        switch (dir)
        {
            case MoveDir::Left:  x -= amount; break;
            case MoveDir::Right: x += amount; break;
            case MoveDir::Up:    y -= amount; break;
            case MoveDir::Down:  y += amount; break;
        }
    };

    // First input = long leg, second input = short leg.
    applyLeg(first.dir, 2, dx, dy);
    applyLeg(second.dir, 1, dx, dy);

    return m_position + sf::Vector2i{dx, dy};
}