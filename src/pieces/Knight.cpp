#include "pieces/ChessPiece.hpp"

Knight::Knight(Color c, sf::Vector2i pos, const sf::Texture& t)
    : ChessPiece(c, PieceType::Knight, pos, t)
{
}
