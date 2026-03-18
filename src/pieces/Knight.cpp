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
    constexpr float chordWindow = 0.08f;

    if (queue.size() < 2)
    {
        return std::nullopt;
    }

    const QueuedInput first = queue[0];
    const QueuedInput second = queue[1];

    const bool closeEnough =
        (second.timeSeconds - first.timeSeconds) <= chordWindow;

    if (!closeEnough)
    {
        queue.pop_front();
        return std::nullopt;
    }

    const bool firstHorizontal =
        first.dir == MoveDir::Left || first.dir == MoveDir::Right;

    const bool secondHorizontal =
        second.dir == MoveDir::Left || second.dir == MoveDir::Right;

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

    applyLeg(first.dir, 2, dx, dy);
    applyLeg(second.dir, 1, dx, dy);

    return m_position + sf::Vector2i{dx, dy};
}

std::optional<sf::Vector2i> Knight::handleDirectionalInput(sf::Keyboard::Key key)
{
    const auto dir = ChessPiece::keyToDir(key);
    if (!dir.has_value())
    {
        return std::nullopt;
    }

    const auto offset = pushKnightInput(*dir);
    if (!offset.has_value())
    {
        return std::nullopt;
    }

    return m_position + *offset;
}

std::optional<sf::Vector2i> Knight::pushKnightInput(MoveDir dir)
{
    if (!m_firstMoveInput.has_value())
    {
        m_firstMoveInput = dir;
        return std::nullopt;
    }

    const MoveDir first = *m_firstMoveInput;
    m_firstMoveInput.reset();

    switch (first)
    {
        case MoveDir::Up:
            if (dir == MoveDir::Left)  return sf::Vector2i{-1, -2};
            if (dir == MoveDir::Right) return sf::Vector2i{ 1, -2};
            break;

        case MoveDir::Down:
            if (dir == MoveDir::Left)  return sf::Vector2i{-1,  2};
            if (dir == MoveDir::Right) return sf::Vector2i{ 1,  2};
            break;

        case MoveDir::Left:
            if (dir == MoveDir::Up)    return sf::Vector2i{-2, -1};
            if (dir == MoveDir::Down)  return sf::Vector2i{-2,  1};
            break;

        case MoveDir::Right:
            if (dir == MoveDir::Up)    return sf::Vector2i{ 2, -1};
            if (dir == MoveDir::Down)  return sf::Vector2i{ 2,  1};
            break;
    }

    return std::nullopt;
}

void Knight::resetInputState()
{
    m_firstMoveInput.reset();
}