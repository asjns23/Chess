#include "pieces/ChessPiece.hpp"

King::King(Color c, sf::Vector2i pos, const sf::Texture& t)
    : ChessPiece(c, PieceType::King, pos, t)
{
}