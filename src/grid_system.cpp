#include "grid_system.h"

obj::obj(std::string type, std::string texName) {
    if (!objTexture.loadFromFile("../../src/Sprites/" + texName )) {
        std::cerr << "ERROR::COULD NOT LOAD FILE::Sprites/" + texName
                  << std::endl;
        throw -1;
    }
    sf::RectangleShape objRectangle({32.f, 32.f});
    objRectangle.setTexture(&objTexture);
    objRectangle.setTextureRect(sf::IntRect({{0, 0}, {32, 32}}));
    objRect = objRectangle;
}

void grid_system::render(sf::RenderWindow& window) {
    for (auto it : grid) {
        for (auto it2 : it) {
            window.draw(it2.objRect);
        }
    }
}

grid_system::grid_system() {
    obj tempObj;
    std::vector<obj> tempVec;
    for (int j = 0; j < 30; j++) {
        tempVec.push_back(tempObj);
    }
    for (int i = 0; i < 60; i++) {
        grid.push_back(tempVec);
    }
}

void grid_system::addObj(obj &object, unsigned x, unsigned y) {
    object.objRect.setPosition({32*x, 32*y});
    grid[x][y] = object;
}

void grid_system::movePos(unsigned x, unsigned y, sf::Vector2i &vMousePosition) {
    this->grid[x][y].objRect.setPosition({(vMousePosition.x/32)*32, (vMousePosition.y/32)*32});
    // this->grid[(vMousePosition.x/32)][(vMousePosition.y/32)] = this->grid[x][y];
    std::swap(this->grid[(vMousePosition.x/32)][(vMousePosition.y/32)], this->grid[x][y]);
}