#include <SFML/Graphics.hpp>
#include <iostream>

enum directions {down, right, up, left};



int main()
{
    unsigned int width = 1920;
    unsigned int height = 1080;

    auto window = sf::RenderWindow(sf::VideoMode({width, height}), "Puzzletorio");
    window.setFramerateLimit(144);

    //Tile system
    sf::Texture tileTexture;

    if(!tileTexture.loadFromFile("../../src/Sprites/tile_32px.png")) {
        std::cerr << "ERROR::COULD NOT LOAD FILE::Sprites/tile_32px.png" << std::endl;
        return -1;
    }
    tileTexture.setRepeated(true);
    sf::RectangleShape tiledArea({1920.f, 960.f});

    tiledArea.setTexture(&tileTexture);
    tiledArea.setTextureRect(sf::IntRect({{0,0}, {1920,960}}));

    //Toolbar
    sf::Texture toolbarTexture;

    if(!toolbarTexture.loadFromFile("../../src/Sprites/toolbar_96px.png")) {
        std::cerr << "ERROR::COULD NOT LOAD FILE::Sprites/toolbar_96px.png" << std::endl;
        return -1;
    }
    toolbarTexture.setRepeated(true);
    sf::RectangleShape toolbarArea({1920.f, 96.f});

    toolbarArea.setTexture(&toolbarTexture);
    toolbarArea.setTextureRect(sf::IntRect({{0,0}, {1920,96}}));
    
    toolbarArea.setPosition({0.f, 972.f});

    //Main loop
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if(const auto* keyPreseed = event->getIf<sf::Event::KeyPressed>()) {
                if(keyPreseed->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }
            }
        }

        //Render
        window.clear(sf::Color(0x404040FF));


        //Draw
        window.draw(tiledArea);
        window.draw(toolbarArea);



        window.display();
    }
}
