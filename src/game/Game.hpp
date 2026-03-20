#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <array>
#include <deque>
#include <memory>
#include <optional>
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
    void update(float dt);
    void updatePlayerAnimation(float dt);
    void render(sf::RenderWindow& window) const;

    void setPlayer(ChessPiece& piece);
    ChessPiece* getPlayer() const;

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

    template<typename PieceT>
    PieceT& emplacePiece(Color color, char file, int rank)
    {
        return emplacePiece<PieceT>(color, chessToBoard(file, rank));
    }

    bool removePiece(sf::Vector2i square);
    bool removePiece(char file, int rank);

    void pruneOldInputs();
    bool attemptPlayerMove(sf::Vector2i targetSquare);
    void processPlayerInput();
    void beginPlayerMoveAnimation(sf::Vector2i targetSquare);

    sf::Vector2f squareToPixel(sf::Vector2i square) const;
    void placePiece(ChessPiece& piece, const sf::Texture& texture);

private:
    enum class InputAction
    {
        Press,
        Release
    };

    struct DirectionInput
    {
        MoveDir dir {};
        InputAction action = InputAction::Press;
        float timeSeconds = 0.0f;
    };

    struct MoveAnimation
    {
        bool active = false;
        sf::Vector2f startPixel {};
        sf::Vector2f endPixel {};
        sf::Vector2i targetSquare {};
        float duration = 0.22f;
        float elapsed = 0.0f;
    };

    struct ShakeAnimation
    {
        bool active = false;
        sf::Vector2f basePixel {};
        float duration = 0.22f;
        float elapsed = 0.0f;
        float amplitude = 6.0f;
    };

    sf::Vector2f piecePixelPosition(const ChessPiece& piece, sf::Vector2i square) const;
    static std::optional<MoveDir> keyToMoveDir(sf::Keyboard::Key key);
    static std::size_t textureIndex(Color color, PieceType type);
    static std::size_t moveDirIndex(MoveDir dir);

    void beginInvalidMoveShake();
    void updatePlayerShakeAnimation(float dt);
    bool isAnimating() const;

    void recordDirectionalInput(MoveDir dir, InputAction action);
    void clearInputState();

    bool loadPieceTextures();
    const sf::Texture& pieceTexture(Color color, PieceType type) const;

    void setupBackground();
    void setupBoard();
    void setupGame();

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    sf::Texture m_boardSquaresTexture;
    sf::Image m_boardSquaresImage;
    std::array<sf::Texture, 12> m_pieceTextures;

    sf::Clock m_inputClock;
    std::unique_ptr<Board> m_board;
    std::vector<std::unique_ptr<ChessPiece>> m_pieces;
    ChessPiece* m_player = nullptr;

    std::deque<DirectionInput> m_inputHistory;
    std::deque<QueuedInput> m_pressQueue;
    std::array<bool, 4> m_heldDirections {false, false, false, false};

    std::optional<sf::Vector2i> m_bufferedClickTarget;
    MoveAnimation m_moveAnimation;
    ShakeAnimation m_shakeAnimation;

    sf::Color m_lightSquareColor {sf::Color::White};
    sf::Color m_darkSquareColor {sf::Color::Black};
};