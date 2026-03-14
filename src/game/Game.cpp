#include "game/Game.hpp"

#include <array>
#include <iostream>
#include <memory>
#include <utility>

#include "game/GameSetup.hpp"

Game::Game(const fs::path& assetPath)
    : m_assetPath(assetPath),
      m_backgroundTexture(),
      m_boardTexture(),
      m_pieceTextures(),
      m_backgroundSprite(m_backgroundTexture),
      m_boardSprite(m_boardTexture)
{
}

bool Game::initialize()
{
    const fs::path backgroundPath = m_assetPath / "board" / "bg.png";
    if (!m_backgroundTexture.loadFromFile(backgroundPath))
    {
        std::cerr << "Failed to load background: " << backgroundPath.string() << "\n";
        return false;
    }

    const fs::path boardPath = m_assetPath / "board" / "board.png";
    if (!m_boardTexture.loadFromFile(boardPath))
    {
        std::cerr << "Failed to load board: " << boardPath.string() << "\n";
        return false;
    }

    if (!loadPieceTextures())
    {
        return false;
    }

    m_backgroundSprite.setTexture(m_backgroundTexture, true);
    m_boardSprite.setTexture(m_boardTexture, true);

    setupBackground();
    setupBoard();
    setupGame();

    return true;
}

bool Game::loadPieceTextures()
{
    struct TextureSpec
    {
        Color color;
        PieceType type;
        const char* filename;
    };

    static constexpr std::array<TextureSpec, 12> specs{{
        {Color::White, PieceType::King,   "white_king.png"},
        {Color::White, PieceType::Queen,  "white_queen.png"},
        {Color::White, PieceType::Rook,   "white_rook.png"},
        {Color::White, PieceType::Bishop, "white_bishop.png"},
        {Color::White, PieceType::Knight, "white_knight.png"},
        {Color::White, PieceType::Pawn,   "white_pawn.png"},
        {Color::Black, PieceType::King,   "black_king.png"},
        {Color::Black, PieceType::Queen,  "black_queen.png"},
        {Color::Black, PieceType::Rook,   "black_rook.png"},
        {Color::Black, PieceType::Bishop, "black_bishop.png"},
        {Color::Black, PieceType::Knight, "black_knight.png"},
        {Color::Black, PieceType::Pawn,   "black_pawn.png"},
    }};

    for (const auto& spec : specs)
    {
        const fs::path path = m_assetPath / "pieces" / spec.filename;
        auto& texture = m_pieceTextures[textureIndex(spec.color, spec.type)];

        if (!texture.loadFromFile(path))
        {
            std::cerr << "Failed to load piece texture: " << path.string() << "\n";
            return false;
        }
    }

    return true;
}

std::size_t Game::textureIndex(Color color, PieceType type)
{
    const std::size_t colorOffset = (color == Color::White) ? 0u : 6u;
    return colorOffset + static_cast<std::size_t>(type);
}

const sf::Texture& Game::pieceTexture(Color color, PieceType type) const
{
    return m_pieceTextures[textureIndex(color, type)];
}

void Game::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.is<sf::Event::Closed>())
    {
        window.close();
    }
}

void Game::render(sf::RenderWindow& window) const
{
    window.draw(m_backgroundSprite);
    window.draw(m_boardSprite);

    for (const auto& piece : m_pieces)
    {
        piece->render(window);
    }
}

sf::Vector2f Game::squareToPixel(sf::Vector2i square) const
{
    constexpr float border = 2.0f;
    constexpr float squareSize = 22.0f;

    return {
        m_boardOrigin.x + (border + static_cast<float>(square.x) * squareSize) * m_boardScaleX,
        m_boardOrigin.y + (border + static_cast<float>(square.y) * squareSize) * m_boardScaleY
    };
}

void Game::placePiece(ChessPiece& piece, const sf::Texture& texture)
{
    constexpr float pieceOffsetX = 3.0f; // (22 - 16) / 2
    constexpr float pieceOffsetY = 2.0f; // (22 - 18) / 2

    piece.setScale({m_boardScaleX, m_boardScaleY});

    const sf::Vector2f squareTopLeft = squareToPixel(piece.getPosition());

    piece.setPixelPosition({
        squareTopLeft.x + pieceOffsetX * m_boardScaleX,
        squareTopLeft.y + pieceOffsetY * m_boardScaleY
    });
}


void Game::setupBackground()
{
    const auto bgSize = m_backgroundTexture.getSize();

    m_backgroundSprite.setScale({
        static_cast<float>(m_windowWidth) / static_cast<float>(bgSize.x),
        static_cast<float>(m_windowHeight) / static_cast<float>(bgSize.y)
    });
}

void Game::setupBoard()
{
    const auto boardTextureSize = m_boardTexture.getSize();

    m_boardScaleX = m_boardPixelSize / static_cast<float>(boardTextureSize.x);
    m_boardScaleY = m_boardPixelSize / static_cast<float>(boardTextureSize.y);

    m_boardOrigin = {
        (static_cast<float>(m_windowWidth) - m_boardPixelSize) * 0.5f,
        (static_cast<float>(m_windowHeight) - m_boardPixelSize) * 0.5f
    };

    m_boardSprite.setScale({m_boardScaleX, m_boardScaleY});
    m_boardSprite.setPosition(m_boardOrigin);
}

void Game::setupGame()
{
    GameSetup::knightTest(*this);
}
