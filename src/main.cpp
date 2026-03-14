#include <SFML/Graphics.hpp>
#include <filesystem>

#include "config.hpp"
#include "game/Game.hpp"


namespace fs = std::filesystem;

int main()
{
    const fs::path assetPath = ASSET_DIR;

    constexpr unsigned int windowWidth = 1280;
    constexpr unsigned int windowHeight = 720;

    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        "Chess"
    );
    window.setFramerateLimit(60);

    Game game(assetPath);
    if (!game.initialize())
    {
        return 1;
    }

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            game.handleEvent(*event, window);
        }

        window.clear();
        game.render(window);
        window.display();
    }

    return 0;
}
