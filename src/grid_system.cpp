#include "./include/grid_system.h"
#include <iostream>

#define PATH "../../src/Sprites" 
#define SPRITE(name) PATH "/Sprites/" name ".png"

// Инициализация статического кэша
std::map<std::string, sf::Texture> GridSystem::textureCache;

sf::Texture* GridSystem::getTexture(const std::string& name) {
    if (name.empty()) return nullptr;
    if (textureCache.find(name) == textureCache.end()) {
        sf::Texture tex;
        if (!tex.loadFromFile(PATH + name)) {
            std::cerr << "Failed to load texture: " << name << std::endl;
            // Возвращаем nullptr, чтобы не кэшировать битую текстуру
            return nullptr; 
        }
        textureCache[name] = tex;
    }
    return &textureCache[name];
}

GameBlock::GameBlock(std::string type, std::string texName) 
    : blockType(type), textureName(texName), rotationState(0), craftingProgress(0), fuelLevel(0) {
    
    blockShape.setSize({32.f, 32.f});
    blockShape.setOrigin({16.f, 16.f}); 

    if (!textureName.empty()) {
        sf::Texture* tex = GridSystem::getTexture(textureName);
        if (tex) {
            blockShape.setTexture(tex);
        }
    }
}

void GameBlock::setRotation(int rot) {
    rotationState = rot % 4;
    // SFML 3.0 FIX: используем sf::degrees
    blockShape.setRotation(sf::degrees(rotationState * 90.f)); 
}

GridSystem::GridSystem() {
    gridMatrix.resize(50, std::vector<GameBlock>(30)); 
}

void GridSystem::placeBlock(GameBlock &block, unsigned gridX, unsigned gridY) {
    if (gridX < gridMatrix.size() && gridY < gridMatrix[0].size()) {
        GameBlock newBlock = block; 
        
        // Гарантируем, что шейп ссылается на валидную текстуру из кэша
        if (!newBlock.textureName.empty()) {
            sf::Texture* tex = GridSystem::getTexture(newBlock.textureName);
            if (tex) newBlock.blockShape.setTexture(tex);
        }

        newBlock.blockShape.setPosition({(float)gridX * 32 + 16, (float)gridY * 32 + 16});
        gridMatrix[gridX][gridY] = newBlock;
    }
}

void GridSystem::swapBlocks(unsigned gridX, unsigned gridY, sf::Vector2i &mousePosition) {
    unsigned targetX = mousePosition.x / 32;
    unsigned targetY = mousePosition.y / 32;

    if (targetX < gridMatrix.size() && targetY < gridMatrix[0].size()) {
        GameBlock temp = gridMatrix[gridX][gridY];
        gridMatrix[gridX][gridY] = gridMatrix[targetX][targetY];
        gridMatrix[targetX][targetY] = temp;

        // Обновляем позиции после обмена
        gridMatrix[gridX][gridY].blockShape.setPosition({(float)gridX * 32 + 16, (float)gridY * 32 + 16});
        gridMatrix[targetX][targetY].blockShape.setPosition({(float)targetX * 32 + 16, (float)targetY * 32 + 16});
    }
}

void GridSystem::clear() {
    GameBlock emptyBlock;
    for (auto& row : gridMatrix) {
        std::fill(row.begin(), row.end(), emptyBlock);
    }
}

void GridSystem::render(sf::RenderWindow& window) {
    for (auto& row : gridMatrix) {
        for (auto& block : row) {
            if (!block.textureName.empty()) {
                // Если вдруг текстура слетела (параноидальная проверка), восстанавливаем
                if (block.blockShape.getTexture() == nullptr) {
                     sf::Texture* tex = GridSystem::getTexture(block.textureName);
                     if (tex) block.blockShape.setTexture(tex);
                }
                window.draw(block.blockShape);
            }
        }
    }
}

void GridSystem::updateGameLogic(unsigned long long currentTick, TaskManager& taskManager) {
    for (int x = 0; x < gridMatrix.size(); ++x) {
        for (int y = 0; y < gridMatrix[x].size(); ++y) {
            GameBlock& block = gridMatrix[x][y];

            // === ЛОГИКА HOPPER ===
            if (block.blockType == "hopper") {
                if (currentTick % 20 == 0) { 
                    int dx = 0, dy = 0;
                    if (block.rotationState == 0) dy = 1;      
                    else if (block.rotationState == 1) dx = -1;
                    else if (block.rotationState == 2) dy = -1;
                    else if (block.rotationState == 3) dx = 1; 

                    int tx = x + dx;
                    int ty = y + dy;

                    if (tx >= 0 && tx < gridMatrix.size() && ty >= 0 && ty < gridMatrix[0].size()) {
                        GameBlock& neighbor = gridMatrix[tx][ty];
                        if (block.inputInventory.itemCount > 0) {
                            bool canTransfer = true;
                            // Проверка Target Block: нужен ли ему этот предмет?
                            if (neighbor.blockType == "target") {
                                if (!taskManager.isNeeded(block.inputInventory.item.itemId)) {
                                    canTransfer = false;
                                }
                            }
                            if (canTransfer) {
                                if (neighbor.inputInventory.addItem(block.inputInventory.item, 1)) {
                                    block.inputInventory.removeItem(1);
                                }
                            }
                        }
                    }
                }
            }

            // === ЛОГИКА TARGET ===
            if (block.blockType == "target") {
                if (block.inputInventory.itemCount > 0) {
                     if (taskManager.submitItem(block.inputInventory.item.itemId)) {
                         block.inputInventory.removeItem(1);
                     }
                }
            }
        }
    }
}
