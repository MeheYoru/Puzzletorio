
sf::Texture texture;

    if(!texture.loadFromFile("../../src/Sprites/tile_32px.png")) {
        std::cerr << "ERROR::COULD NOT LOAD FILE::prites/tile_32px.png" << std::endl;
        return -1;
    }
    texture.setRepeated(true);
    sf::RectangleShape tiledArea({1920.f, 960.f});

    tiledArea.setTexture(&texture);
    tiledArea.setTextureRect(sf::IntRect({{0,0}, {1920,960}}));