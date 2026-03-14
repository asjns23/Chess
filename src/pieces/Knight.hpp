#pragma once

#include "pieces/ChessPiece.hpp"

class Knight : public ChessPiece
{
public:
    static constexpr PieceType type = PieceType::Knight;

    Knight(Color c, sf::Vector2i pos, const sf::Texture& t);
};
