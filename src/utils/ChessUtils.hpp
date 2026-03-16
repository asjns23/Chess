#pragma once

#include <SFML/Graphics.hpp>
#include <string_view>

sf::Vector2i chessToBoard(int file, int rank);
sf::Vector2i chessToBoard(char file, int rank);
sf::Vector2i chessToBoard(const char* square);
sf::Vector2i chessToBoard(std::string_view square);

sf::Vector2f boardToPixel(sf::Vector2i boardPos, float squareSize);