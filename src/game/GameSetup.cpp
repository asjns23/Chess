#include "game/GameSetup.hpp"

#include "game/Game.hpp"
#include "utils/ChessUtils.hpp"

void GameSetup::knightTest(Game& game)
{
    for (int i = 0; i < 8; ++i){
        for (int j = 0; j < 8; ++j){
            const char file = 'A' + i;
            const char rank = 1 + j;
            const sf::Vector2i square = chessToBoard(file, rank);
            game.emplacePiece<Knight>(Color::White, square);
        }
    }
}
