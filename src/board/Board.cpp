#include "board/Board.hpp"

/// Constructs the board with the given configuration, texture, and colors
Board::Board(sf::Vector2f origin,
             const Config::Board& config,
             const sf::Texture& boardSquaresTexture,
             sf::Color lightColor,
             sf::Color darkColor)
    : m_origin(origin),
      m_columns(config.columns),
      m_rows(config.rows),
      m_squareSize(config.squareSize),
      m_border(config.border),
      m_boardSquaresTexture(&boardSquaresTexture),
      m_lightColor(lightColor),
      m_darkColor(darkColor),
      m_lightRect(),
      m_darkRect(),
      m_borderShape(),
      m_squareSprites()
{
    const sf::Vector2u textureSize = m_boardSquaresTexture->getSize();

    const int tileWidth = static_cast<int>(textureSize.x / 2u);
    const int tileHeight = static_cast<int>(textureSize.y);

    m_lightRect = sf::IntRect({0, 0}, {tileWidth, tileHeight});
    m_darkRect  = sf::IntRect({tileWidth, 0}, {tileWidth, tileHeight});

    m_borderShape.setPosition(m_origin);
    m_borderShape.setSize({getWidth(), getHeight()});
    m_borderShape.setFillColor(m_lightColor);

    buildSprites();
}

// Checks if the given square coordinates are within the bounds of the board
bool Board::isInside(sf::Vector2i square) const
{
    return square.x >= 0 && square.x < m_columns &&
           square.y >= 0 && square.y < m_rows;
}

// Converts square coordinates (e.g., (0, 0) for top-left) to pixel coordinates for rendering
sf::Vector2f Board::squareToPixel(sf::Vector2i square) const
{
    return {
        m_origin.x + m_border + static_cast<float>(square.x) * m_squareSize,
        m_origin.y + m_border + static_cast<float>(square.y) * m_squareSize
    };
}

// Converts pixel coordinates (e.g., from mouse position) to square coordinates on the board
sf::Vector2i Board::pixelToSquare(sf::Vector2i pixel) const
{
    return {
        static_cast<int>((static_cast<float>(pixel.x) - m_origin.x - m_border) / m_squareSize),
        static_cast<int>((static_cast<float>(pixel.y) - m_origin.y - m_border) / m_squareSize)
    };
}

// Returns the total width of the board in pixels, including borders
float Board::getWidth() const
{
    return 2.0f * m_border + static_cast<float>(m_columns) * m_squareSize;
}

// Returns the total height of the board in pixels, including borders
float Board::getHeight() const
{
    return 2.0f * m_border + static_cast<float>(m_rows) * m_squareSize;
}

// Getter for the color of light squares on the board
sf::Color Board::getLightColor() const
{
    return m_lightColor;
}

// Getter for the color of dark squares on the board
sf::Color Board::getDarkColor() const
{
    return m_darkColor;
}

// Builds custom board sprite using dark and light square sprites, scaled to fit board dimensions
void Board::buildSprites()
{
    m_squareSprites.clear();
    m_squareSprites.reserve(static_cast<std::size_t>(m_columns * m_rows));

    const float sourceWidth = static_cast<float>(m_lightRect.size.x);
    const float sourceHeight = static_cast<float>(m_lightRect.size.y);

    const float scaleX = m_squareSize / sourceWidth;
    const float scaleY = m_squareSize / sourceHeight;

    for (int row = 0; row < m_rows; ++row)
    {
        for (int col = 0; col < m_columns; ++col)
        {
            const bool isLight = ((row + col) % 2 == 0);

            sf::Sprite sprite(*m_boardSquaresTexture);
            sprite.setTextureRect(isLight ? m_lightRect : m_darkRect);
            sprite.setScale({scaleX, scaleY});
            sprite.setPosition(squareToPixel({col, row}));

            m_squareSprites.push_back(sprite);
        }
    }
}

// Draws the board by first drawing the border and then all the square sprites
void Board::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_borderShape, states);

    for (const auto& sprite : m_squareSprites)
    {
        target.draw(sprite, states);
    }
}