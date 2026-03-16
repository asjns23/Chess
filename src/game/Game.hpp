#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <vector>

#include "board/Board.hpp"
#include "pieces/ChessPiece.hpp"
#include "utils/ChessUtils.hpp"

class Game
{
public:
    Game();

    bool initialize();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void render(sf::RenderWindow& window) const;

    template<typename PieceT>
    PieceT& emplacePiece(Color color, sf::Vector2i square)
    {
        const sf::Texture& texture = pieceTexture(color, PieceT::type);

        auto piece = std::make_unique<PieceT>(color, square, texture);
        PieceT& ref = *piece;

        placePiece(ref, texture);
        m_pieces.push_back(std::move(piece));

        return ref;
    }

    // overload for chess notation
    template<typename PieceT>
    PieceT& emplacePiece(Color color, char file, int rank)
    {
        return emplacePiece<PieceT>(color, chessToBoard(file, rank));
    }

    bool removePiece(sf::Vector2i square);
    bool removePiece(char file, int rank); // overload for chess notation

    sf::Vector2f squareToPixel(sf::Vector2i square) const;
    void placePiece(ChessPiece& piece, const sf::Texture& texture);

private:
    bool loadPieceTextures();
    const sf::Texture& pieceTexture(Color color, PieceType type) const;
    static std::size_t textureIndex(Color color, PieceType type);

    void setupBackground();
    void setupBoard();
    void setupGame();

private:
    sf::Texture m_backgroundTexture;
    sf::Texture m_boardSquaresTexture;
    sf::Image m_boardSquaresImage;
    std::array<sf::Texture, 12> m_pieceTextures;

    sf::Sprite m_backgroundSprite;

    std::unique_ptr<Board> m_board;
    std::vector<std::unique_ptr<ChessPiece>> m_pieces;

    sf::Color m_lightSquareColor {sf::Color::White};
    sf::Color m_darkSquareColor {sf::Color::Black};
};