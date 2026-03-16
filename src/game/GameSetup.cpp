#include "game/GameSetup.hpp"

#include "Config.hpp"
#include "game/Game.hpp"
#include "utils/ChessUtils.hpp"

void GameSetup::knightTest(Game& game)
{
    for (int y = 0; y < Config::board.rows; ++y)
    {
        for (int x = 0; x < Config::board.columns; ++x)
        {
            game.emplacePiece<Knight>(Color::White, {x, y});
        }
    }
    game.removePiece('A', 4);
    game.emplacePiece<King>(Color::Black, 'A', 4);
}
