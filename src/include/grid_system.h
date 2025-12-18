#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
#include "item.h"
#include "task_system.h"

class GameBlock
{
public:
    sf::RectangleShape blockShape;
    std::string blockType;
    std::string textureName;
    int rotationState;

    Inventory inputInventory;
    Inventory outputInventory;
    Inventory fuelInventory;

    int craftingProgress;
    float fuelLevel;

    GameBlock(std::string type, std::string texName);
    GameBlock() : rotationState(0), craftingProgress(0), fuelLevel(0) {};

    void setRotation(int rot);
};

class GridSystem
{
public:
    std::vector<std::vector<GameBlock>> gridMatrix;
    static std::map<std::string, sf::Texture> textureCache;
    static sf::Texture *getTexture(const std::string &name);

    GridSystem();
    void placeBlock(GameBlock &block, unsigned gridX, unsigned gridY);
    void render(sf::RenderWindow &window);
    void swapBlocks(unsigned gridX, unsigned gridY, sf::Vector2i &mousePosition);
    void updateGameLogic(unsigned long long currentTick, TaskManager &taskManager);
    void clear();
};
