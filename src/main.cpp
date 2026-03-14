#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include "config.hpp"


namespace fs = std::filesystem;

int main()
{
    fs::path assetPath = ASSET_DIR;

    constexpr unsigned int windowWidth = 1280;
    constexpr unsigned int windowHeight = 720;

    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        "Chess"
    );
    window.setFramerateLimit(60);

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile(assetPath / "board" / "bg.png"))
    {
        std::cerr << "Failed to load background: " << (assetPath / "board" / "bg.png").string() << "\n";
        return 1;
    }

    sf::Texture boardTexture;
    if (!boardTexture.loadFromFile(assetPath / "board" / "board.png"))
    {
        std::cerr << "Failed to load board: " << (assetPath / "board" / "board.png").string() << "\n";
        return 1;
    }

    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Sprite boardSprite(boardTexture);

    // Scale background to fill window
    {
        const auto bgSize = backgroundTexture.getSize();
        backgroundSprite.setScale({
            static_cast<float>(windowWidth) / static_cast<float>(bgSize.x),
            static_cast<float>(windowHeight) / static_cast<float>(bgSize.y)
        });
    }

    // Scale board to fit nicely inside the window
    constexpr float boardPixelSize = 640.0f;
    {
        const auto boardSize = boardTexture.getSize();
        boardSprite.setScale({
            boardPixelSize / static_cast<float>(boardSize.x),
            boardPixelSize / static_cast<float>(boardSize.y)
        });
    }

    // Center board in window
    boardSprite.setPosition({
        (static_cast<float>(windowWidth) - boardPixelSize) * 0.5f,
        (static_cast<float>(windowHeight) - boardPixelSize) * 0.5f
    });

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(boardSprite);
        window.display();
    }

    return 0;
}
