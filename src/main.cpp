#include <SFML/Graphics.hpp>
#include <filesystem>

#include "Config.hpp"
#include "game/Game.hpp"


namespace fs = std::filesystem;

int main()
{
    // Window configuration
    const unsigned int windowWidth  = Config::window.width;
    const unsigned int windowHeight = Config::window.height;
    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        Config::window.title
    );
    window.setFramerateLimit(60);

    // Game setup
    Game game;
    if (!game.initialize())
    {
        return -1;
    }

    // Main loop
    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            game.handleEvent(*event, window);
        }

        window.clear();
        game.render(window);
        window.display();
    }

    return 0;
}
