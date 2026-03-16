#include "game/Game.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <utility>

#include "Config.hpp"
#include "game/GameSetup.hpp"
#include "utils/ChessUtils.hpp"

namespace fs = std::filesystem;

namespace
{
    std::pair<sf::Color, sf::Color> sampleBoardColors(const sf::Image& image)
    {
        const sf::Vector2u size = image.getSize();

        if (size.x < 2 || size.y < 1)
        {
            return {sf::Color::White, sf::Color::Black};
        }

        const unsigned int sampleY = size.y / 2;
        const unsigned int lightX  = size.x / 4;
        const unsigned int darkX   = (3 * size.x) / 4;

        const sf::Color lightColor = image.getPixel({lightX, sampleY});
        const sf::Color darkColor  = image.getPixel({darkX, sampleY});

        return {lightColor, darkColor};
    }
}

Game::Game()
    : m_backgroundTexture(),
      m_boardSquaresTexture(),
      m_boardSquaresImage(),
      m_pieceTextures(),
      m_backgroundSprite(m_backgroundTexture),
      m_board(),
      m_pieces(),
      m_lightSquareColor(sf::Color::White),
      m_darkSquareColor(sf::Color::Black)
{
}

// Loads all necessary textures and sets up the game state. Returns true on success.
bool Game::initialize()
{
    const fs::path backgroundPath =
        fs::path(Config::Paths::BoardDir) / Config::Paths::BackgroundFile;

    if (!m_backgroundTexture.loadFromFile(backgroundPath))
    {
        std::cerr << "Failed to load background: " << backgroundPath.string() << "\n";
        return false;
    }

    const fs::path boardSquaresPath =
        fs::path(Config::Paths::BoardDir) / Config::Paths::BoardSquaresFile;

    if (!m_boardSquaresTexture.loadFromFile(boardSquaresPath))
    {
        std::cerr << "Failed to load board squares texture: "
                  << boardSquaresPath.string() << "\n";
        return false;
    }

    if (!m_boardSquaresImage.loadFromFile(boardSquaresPath))
    {
        std::cerr << "Failed to load board squares image: "
                  << boardSquaresPath.string() << "\n";
        return false;
    }

    std::tie(m_lightSquareColor, m_darkSquareColor) =
        sampleBoardColors(m_boardSquaresImage);

    if (!loadPieceTextures())
    {
        return false;
    }

    m_backgroundSprite.setTexture(m_backgroundTexture, true);

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

    // Define PieceTypes and corresponding texture filenames for easy management
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
        const fs::path path =
            fs::path(Config::Paths::PiecesDir) / spec.filename;

        auto& texture = m_pieceTextures[textureIndex(spec.color, spec.type)];

        if (!texture.loadFromFile(path))
        {
            std::cerr << "Failed to load piece texture: " << path.string() << "\n";
            return false;
        }
    }

    return true;
}

// Helper function to calculate the index in m_pieceTextures for a given color and piece type
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

    if (m_board)
    {
        window.draw(*m_board);
    }

    for (const auto& piece : m_pieces)
    {
        piece->render(window);
    }
}

// Converts a board square (file, rank) to pixel coordinates for rendering
sf::Vector2f Game::squareToPixel(sf::Vector2i square) const
{
    return m_board->squareToPixel(square);
}

// Places a piece on the board and sets its pixel position based on its square position
void Game::placePiece(ChessPiece& piece, const sf::Texture& texture)
{
    const sf::Vector2f squareTopLeft = squareToPixel(piece.getPosition());
    const sf::Vector2u texSize = texture.getSize();

    // Reference board square size from the original art
    constexpr float baseSquareSize = 22.0f;

    // Current board square size from config
    const float squareSize = Config::board.squareSize;

    // Uniform board scale derived from how much the board grew/shrank
    const float boardScale = squareSize / baseSquareSize;

    piece.setScale({boardScale, boardScale});

    const float drawnWidth = static_cast<float>(texSize.x) * boardScale;
    const float drawnHeight = static_cast<float>(texSize.y) * boardScale;

    const float offsetX = (squareSize - drawnWidth) * 0.5f;
    const float offsetY = (squareSize - drawnHeight) * 0.5f;

    piece.setPixelPosition({
        squareTopLeft.x + offsetX,
        squareTopLeft.y + offsetY
    });
}

// Removes a piece from the board at the given square. Returns false if no piece was found at that square.
bool Game::removePiece(sf::Vector2i square)
{
    const auto it = std::find_if(
        m_pieces.begin(),
        m_pieces.end(),
        [square](const std::unique_ptr<ChessPiece>& piece)
        {
            return piece->getPosition() == square;
        });

    if (it == m_pieces.end())
    {
        return false;
    }

    m_pieces.erase(it);
    return true;
}

// Overload for chess notation
bool Game::removePiece(char file, int rank)
{
    return removePiece(chessToBoard(file, rank));
}

// Sets up the background sprite to cover the entire window
void Game::setupBackground()
{
    const sf::Vector2u bgSize = m_backgroundTexture.getSize();

    m_backgroundSprite.setScale({
        static_cast<float>(Config::window.width) / static_cast<float>(bgSize.x),
        static_cast<float>(Config::window.height) / static_cast<float>(bgSize.y)
    });
}

// Sets up the board by calculating its pixel position and creating a Board object
void Game::setupBoard()
{
    const float boardWidth =
        2.0f * Config::board.border +
        static_cast<float>(Config::board.columns) * Config::board.squareSize;

    const float boardHeight =
        2.0f * Config::board.border +
        static_cast<float>(Config::board.rows) * Config::board.squareSize;

    const sf::Vector2f origin{
        (static_cast<float>(Config::window.width) - boardWidth) * 0.5f,
        (static_cast<float>(Config::window.height) - boardHeight) * 0.5f
    };

    m_board = std::make_unique<Board>(
        origin,
        Config::board,
        m_boardSquaresTexture,
        m_lightSquareColor,
        m_darkSquareColor
    );
}

void Game::setupGame()
{
    GameSetup::knightTest(*this);
}