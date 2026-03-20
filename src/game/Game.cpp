#include "game/Game.hpp"

#include <algorithm>
#include <array>
#include <cmath>
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

    sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t)
    {
        return a + (b - a) * t;
    }

    float smoothStep(float t)
    {
        return t * t * (3.0f - 2.0f * t);
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
      m_player(nullptr),
      m_inputHistory(),
      m_pressQueue(),
      m_heldDirections{false, false, false, false},
      m_moveAnimation(),
      m_shakeAnimation(),
      m_lightSquareColor(sf::Color::White),
      m_darkSquareColor(sf::Color::Black)
{
}

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

std::size_t Game::textureIndex(Color color, PieceType type)
{
    const std::size_t colorOffset = (color == Color::White) ? 0u : 6u;
    return colorOffset + static_cast<std::size_t>(type);
}

std::size_t Game::moveDirIndex(MoveDir dir)
{
    switch (dir)
    {
        case MoveDir::Up:    return 0u;
        case MoveDir::Down:  return 1u;
        case MoveDir::Left:  return 2u;
        case MoveDir::Right: return 3u;
    }

    return 0u;
}

const sf::Texture& Game::pieceTexture(Color color, PieceType type) const
{
    return m_pieceTextures[textureIndex(color, type)];
}

void Game::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (const auto* closed = event.getIf<sf::Event::Closed>())
    {
        window.close();
        return;
    }

    if (!m_player)
    {
        return;
    }

    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button != sf::Mouse::Button::Left)
        {
            return;
        }

        const sf::Vector2i mousePixel = mousePressed->position;
        if (!m_board->containsPixel(mousePixel))
        {
            return;
        }

        const sf::Vector2i targetSquare = m_board->pixelToSquare(mousePixel);

        clearInputState();
        m_player->resetInputState();

        if (isAnimating())
        {
            m_bufferedClickTarget = targetSquare;
            return;
        }

        if (!attemptPlayerMove(targetSquare))
        {
            beginInvalidMoveShake();
        }

        return;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        const auto dir = keyToMoveDir(keyPressed->code);
        if (!dir.has_value())
        {
            return;
        }

        const std::size_t index = moveDirIndex(*dir);
        if (m_heldDirections[index])
        {
            return;
        }

        recordDirectionalInput(*dir, InputAction::Press);
        return;
    }

    if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        const auto dir = keyToMoveDir(keyReleased->code);
        if (!dir.has_value())
        {
            return;
        }

        recordDirectionalInput(*dir, InputAction::Release);

        if (!isAnimating())
        {
            processPlayerInput();
        }

        return;
    }
}

void Game::update(float dt)
{
    updatePlayerAnimation(dt);
    updatePlayerShakeAnimation(dt);

    if (!isAnimating())
    {
        processPlayerInput();
    }
}

void Game::updatePlayerAnimation(float dt)
{
    if (!m_moveAnimation.active || !m_player)
    {
        return;
    }

    m_moveAnimation.elapsed += dt;

    float t = m_moveAnimation.elapsed / m_moveAnimation.duration;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    const float eased = smoothStep(t);
    const sf::Vector2f pixelPos =
        lerp(m_moveAnimation.startPixel, m_moveAnimation.endPixel, eased);

    m_player->setPixelPosition(pixelPos);

    if (t >= 1.0f)
    {
        m_player->move(m_moveAnimation.targetSquare);
        m_player->setPixelPosition(m_moveAnimation.endPixel);
        m_moveAnimation.active = false;

        processPlayerInput();
    }
}

void Game::updatePlayerShakeAnimation(float dt)
{
    if (!m_shakeAnimation.active || !m_player)
    {
        return;
    }

    m_shakeAnimation.elapsed += dt;

    float t = m_shakeAnimation.elapsed / m_shakeAnimation.duration;
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    const float fade = 1.0f - t;
    const float wave = std::sin(t * 20.0f * 3.14159265f);
    const float offsetX = wave * m_shakeAnimation.amplitude * fade;

    m_player->setPixelPosition({
        m_shakeAnimation.basePixel.x + offsetX,
        m_shakeAnimation.basePixel.y
    });

    if (t >= 1.0f)
    {
        m_player->setPixelPosition(m_shakeAnimation.basePixel);
        m_shakeAnimation.active = false;
    }
}

std::optional<MoveDir> Game::keyToMoveDir(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::Up:
            return MoveDir::Up;

        case sf::Keyboard::Key::S:
        case sf::Keyboard::Key::Down:
            return MoveDir::Down;

        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::Left:
            return MoveDir::Left;

        case sf::Keyboard::Key::D:
        case sf::Keyboard::Key::Right:
            return MoveDir::Right;

        default:
            return std::nullopt;
    }
}

void Game::recordDirectionalInput(MoveDir dir, InputAction action)
{
    constexpr std::size_t MaxInputHistorySize = 16;
    constexpr std::size_t MaxPressQueueSize = 8;

    pruneOldInputs();

    const float now = m_inputClock.getElapsedTime().asSeconds();

    if (m_inputHistory.size() >= MaxInputHistorySize)
    {
        m_inputHistory.pop_front();
    }

    m_inputHistory.push_back(DirectionInput{
        .dir = dir,
        .action = action,
        .timeSeconds = now
    });

    m_heldDirections[moveDirIndex(dir)] = (action == InputAction::Press);

    if (action == InputAction::Press)
    {
        if (m_pressQueue.size() >= MaxPressQueueSize)
        {
            m_pressQueue.pop_front();
        }

        m_pressQueue.push_back(QueuedInput{
            .dir = dir,
            .timeSeconds = now
        });
    }
}

void Game::clearInputState()
{
    m_inputHistory.clear();
    m_pressQueue.clear();
    m_heldDirections = {false, false, false, false};
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

void Game::setPlayer(ChessPiece& piece)
{
    m_player = &piece;
    m_player->setPixelPosition(piecePixelPosition(*m_player, m_player->getPosition()));
}

ChessPiece* Game::getPlayer() const
{
    return m_player;
}

sf::Vector2f Game::squareToPixel(sf::Vector2i square) const
{
    return m_board->squareToPixel(square);
}

sf::Vector2f Game::piecePixelPosition(const ChessPiece& piece, sf::Vector2i square) const
{
    const sf::Vector2f squareTopLeft = squareToPixel(square);
    const sf::Texture& texture = pieceTexture(piece.getColor(), piece.getType());
    const sf::Vector2u texSize = texture.getSize();

    constexpr float baseSquareSize = 22.0f;
    const float squareSize = Config::board.squareSize;
    const float boardScale = squareSize / baseSquareSize;

    const float drawnWidth = static_cast<float>(texSize.x) * boardScale;
    const float drawnHeight = static_cast<float>(texSize.y) * boardScale;

    const float offsetX = (squareSize - drawnWidth) * 0.5f;
    const float offsetY = (squareSize - drawnHeight) * 0.5f;

    return {
        squareTopLeft.x + offsetX,
        squareTopLeft.y + offsetY
    };
}

void Game::placePiece(ChessPiece& piece, const sf::Texture&)
{
    constexpr float baseSquareSize = 22.0f;
    const float squareSize = Config::board.squareSize;
    const float boardScale = squareSize / baseSquareSize;

    piece.setScale({boardScale, boardScale});
    piece.setPixelPosition(piecePixelPosition(piece, piece.getPosition()));
}

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

    if (it->get() == m_player)
    {
        m_player = nullptr;
        m_moveAnimation.active = false;
        m_shakeAnimation.active = false;
        clearInputState();
    }

    m_pieces.erase(it);
    return true;
}

bool Game::removePiece(char file, int rank)
{
    return removePiece(chessToBoard(file, rank));
}

bool Game::attemptPlayerMove(sf::Vector2i targetSquare)
{
    if (!m_player)
    {
        return false;
    }

    if (!m_board->isInside(targetSquare))
    {
        return false;
    }

    if (!m_player->isValidMove(targetSquare))
    {
        return false;
    }

    beginPlayerMoveAnimation(targetSquare);
    return true;
}

void Game::pruneOldInputs()
{
    constexpr float maxAgeSeconds = 0.35f;
    const float now = m_inputClock.getElapsedTime().asSeconds();

    while (!m_inputHistory.empty())
    {
        const float age = now - m_inputHistory.front().timeSeconds;
        if (age <= maxAgeSeconds)
        {
            break;
        }

        m_inputHistory.pop_front();
    }

    while (!m_pressQueue.empty())
    {
        const float age = now - m_pressQueue.front().timeSeconds;
        if (age <= maxAgeSeconds)
        {
            break;
        }

        m_pressQueue.pop_front();
    }
}

void Game::processPlayerInput()
{
    if (!m_player || isAnimating())
    {
        return;
    }

    pruneOldInputs();

    if (m_bufferedClickTarget.has_value())
    {
        const sf::Vector2i targetSquare = *m_bufferedClickTarget;
        m_bufferedClickTarget.reset();

        if (!attemptPlayerMove(targetSquare))
        {
            beginInvalidMoveShake();
        }

        return;
    }

    if (m_pressQueue.empty())
    {
        return;
    }

    const float now = m_inputClock.getElapsedTime().asSeconds();
    const float newestAge = now - m_pressQueue.back().timeSeconds;
    constexpr float inputSettleDelay = 0.09f;

    if (newestAge < inputSettleDelay)
    {
        return;
    }

    while (!m_pressQueue.empty() && !isAnimating())
    {
        const std::size_t sizeBefore = m_pressQueue.size();

        const auto target = m_player->nextMove(m_pressQueue);
        if (!target.has_value())
        {
            if (m_pressQueue.size() == sizeBefore)
            {
                break;
            }

            continue;
        }

        if (!attemptPlayerMove(*target))
        {
            beginInvalidMoveShake();
            break;
        }
    }
}

void Game::beginPlayerMoveAnimation(sf::Vector2i targetSquare)
{
    if (!m_player)
    {
        return;
    }

    m_moveAnimation.active = true;
    m_moveAnimation.elapsed = 0.0f;
    m_moveAnimation.targetSquare = targetSquare;
    m_moveAnimation.startPixel =
        piecePixelPosition(*m_player, m_player->getPosition());
    m_moveAnimation.endPixel =
        piecePixelPosition(*m_player, targetSquare);
}

void Game::beginInvalidMoveShake()
{
    if (!m_player)
    {
        return;
    }

    m_shakeAnimation.active = true;
    m_shakeAnimation.elapsed = 0.0f;
    m_shakeAnimation.basePixel =
        piecePixelPosition(*m_player, m_player->getPosition());
}

bool Game::isAnimating() const
{
    return m_moveAnimation.active || m_shakeAnimation.active;
}

void Game::setupBackground()
{
    const sf::Vector2u bgSize = m_backgroundTexture.getSize();

    m_backgroundSprite.setScale({
        static_cast<float>(Config::window.width) / static_cast<float>(bgSize.x),
        static_cast<float>(Config::window.height) / static_cast<float>(bgSize.y)
    });
}

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
    // GameSetup::kingTest(*this);
}