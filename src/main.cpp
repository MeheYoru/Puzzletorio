#include <SFML/Graphics.hpp>
#include <iostream>
<<<<<<< HEAD
=======
#include "grid_system.h"

#define PATH "../../src"
#define SPRITE(name) PATH "/Sprites/" name ".png"
>>>>>>> 446b816 (Basic grid and object handling)

enum directions {down, right, up, left};



int main()
{
    unsigned int width = 1920;
    unsigned int height = 1080;

    auto window = sf::RenderWindow(sf::VideoMode({width, height}), "Puzzletorio");
    window.setFramerateLimit(144);

    //Tile system
    sf::Texture tileTexture;

<<<<<<< HEAD
    if(!tileTexture.loadFromFile("../../src/Sprites/tile_32px.png")) {
        std::cerr << "ERROR::COULD NOT LOAD FILE::Sprites/tile_32px.png" << std::endl;
        return -1;
    }
=======
    if(!tileTexture.loadFromFile(SPRITE("tile_32px"))) {
        std::cerr << "ERROR::COULD NOT LOAD FILE::Sprites/tile_32px.png" << std::endl;
        return -1;
    }

>>>>>>> 446b816 (Basic grid and object handling)
    tileTexture.setRepeated(true);
    sf::RectangleShape tiledArea({1920.f, 960.f});

    tiledArea.setTexture(&tileTexture);
    tiledArea.setTextureRect(sf::IntRect({{0,0}, {1920,960}}));

<<<<<<< HEAD
    //Toolbar
    sf::Texture toolbarTexture;

    if(!toolbarTexture.loadFromFile("../../src/Sprites/toolbar_96px.png")) {
=======
    sf::Texture tileHighlightedTexture;
    
    if(!tileHighlightedTexture.loadFromFile(SPRITE("tile_highlighted_32px"))) {
        std::cerr << "ERROR::COULD NOT LOAD FILE::Sprites/tile_highlighted_32px.png" << std::endl;
        return -1;
    }
    
    sf::RectangleShape tileHighlighted({32.f, 32.f});
    tileHighlighted.setTexture(&tileHighlightedTexture);
    tileHighlighted.setTextureRect(sf::IntRect({{0,0}, {32,32}}));
    tileHighlighted.setFillColor(sf::Color(0x00000044));



    //Toolbar
    sf::Texture toolbarTexture;

    if(!toolbarTexture.loadFromFile(SPRITE("toolbar_96px"))) {
>>>>>>> 446b816 (Basic grid and object handling)
        std::cerr << "ERROR::COULD NOT LOAD FILE::Sprites/toolbar_96px.png" << std::endl;
        return -1;
    }
    toolbarTexture.setRepeated(true);
    sf::RectangleShape toolbarArea({1920.f, 96.f});

    toolbarArea.setTexture(&toolbarTexture);
    toolbarArea.setTextureRect(sf::IntRect({{0,0}, {1920,96}}));
    
    toolbarArea.setPosition({0.f, 972.f});

<<<<<<< HEAD
=======

    //Temp
    grid_system Grid;

    obj Furnace("furnace", "/block/furnace_front_on.png");
    obj Hopper("hopper", "/item/hopper.png");
    obj Iron_source("iron_source", "/block/iron_block.png");
    
    Grid.addObj(Hopper, 15, 6);
    Grid.addObj(Furnace, 15, 7);
    Grid.addObj(Iron_source, 15, 5);




>>>>>>> 446b816 (Basic grid and object handling)
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
<<<<<<< HEAD
=======
    sf::Vector2i vMousePosition = sf::Mouse::getPosition(window);
    auto pos32 = sf::Vector2f((vMousePosition.x/32)*32, (vMousePosition.y/32)*32);
    if(pos32.y < 960) {
        tileHighlighted.setPosition(pos32);
    }

>>>>>>> 446b816 (Basic grid and object handling)

        //Render
        window.clear(sf::Color(0x404040FF));


<<<<<<< HEAD
        //Draw
        window.draw(tiledArea);
        window.draw(toolbarArea);


=======
        // Draw
        window.draw(tiledArea);
        window.draw(toolbarArea);;

        Grid.render(window);

        window.draw(tileHighlighted);
>>>>>>> 446b816 (Basic grid and object handling)

        window.display();
    }
}
