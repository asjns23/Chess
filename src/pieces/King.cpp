#include "pieces/ChessPiece.hpp"

#include <cstdlib>

King::King(Color c, sf::Vector2i pos, const sf::Texture& t)
    : ChessPiece(c, PieceType::King, pos, t)
{
}

bool King::isValidMove(sf::Vector2i newPos) const
{
    const int dx = std::abs(newPos.x - m_position.x);
    const int dy = std::abs(newPos.y - m_position.y);

    return (dx == 1 && dy == 1) || (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

std::optional<sf::Vector2i> King::nextMove(std::deque<QueuedInput>& queue) const
{
    constexpr float combinedInputWindow = 0.08f;

    if (queue.empty())
    {
        return std::nullopt;
    }

    const QueuedInput first = queue.front();
    queue.pop_front();

    int dx = 0;
    int dy = 0;

    auto applyDir = [](MoveDir dir, int& x, int& y)
    {
        switch (dir)
        {
            case MoveDir::Left:  x = -1; break;
            case MoveDir::Right: x =  1; break;
            case MoveDir::Up:    y = -1; break;
            case MoveDir::Down:  y =  1; break;
        }
    };

    applyDir(first.dir, dx, dy);

    if (!queue.empty())
    {
        const QueuedInput second = queue.front();

        const bool closeEnough =
            (second.timeSeconds - first.timeSeconds) <= combinedInputWindow;

        const bool firstHorizontal =
            first.dir == MoveDir::Left || first.dir == MoveDir::Right;

        const bool secondHorizontal =
            second.dir == MoveDir::Left || second.dir == MoveDir::Right;

        const bool differentAxis = firstHorizontal != secondHorizontal;

        if (closeEnough && differentAxis)
        {
            queue.pop_front();
            applyDir(second.dir, dx, dy);
        }
    }

    return m_position + sf::Vector2i{dx, dy};
}