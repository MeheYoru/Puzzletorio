#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


class obj {
   public:
    sf::Texture objTexture;
    sf::RectangleShape objRect;
    obj(std::string type, std::string texName);
    obj() {};
};

class grid_system {
   public:
    void addObj(obj object, unsigned x, unsigned y);
    grid_system();
    std::vector<std::vector<obj>> grid;
    void render(sf::RenderWindow& window);
};
