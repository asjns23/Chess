#include "game/GameSetup.hpp"

#include "Config.hpp"
#include "game/Game.hpp"
#include "utils/ChessUtils.hpp"

void GameSetup::knightTest(Game& game)
{
    auto& playerKnight = game.emplacePiece<Knight>(Color::Black, 'A', 4);
    game.setPlayer(playerKnight);
}
