#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <filesystem>
#include <memory>
#include <vector>

#include "pieces/ChessPiece.hpp"
#include "pieces/Knight.hpp"

namespace fs = std::filesystem;

class Game
{
public:
    explicit Game(const fs::path& assetPath);

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
    fs::path m_assetPath;

    static constexpr unsigned int m_windowWidth = 1280;
    static constexpr unsigned int m_windowHeight = 720;
    static constexpr float m_boardPixelSize = 640.0f;
    static constexpr float m_boardBorderPixels = 1.0f;

    sf::Texture m_backgroundTexture;
    sf::Texture m_boardTexture;
    std::array<sf::Texture, 12> m_pieceTextures;

    sf::Sprite m_backgroundSprite;
    sf::Sprite m_boardSprite;

    std::vector<std::unique_ptr<ChessPiece>> m_pieces;

    sf::Vector2f m_boardOrigin {};
    float m_boardScaleX {};
    float m_boardScaleY {};


};