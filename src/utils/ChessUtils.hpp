#pragma once

#include <SFML/System/Vector2.hpp>

sf::Vector2i chessToBoard(char file, int rank);
sf::Vector2f boardToPixel(sf::Vector2i boardPos, sf::Vector2f tileSize);