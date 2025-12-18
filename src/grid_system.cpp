#include "./include/grid_system.h"
#include <iostream>
#include <vector>

#define PATH "../../src" 

// ... (Структура Recipe и список рецептов без изменений) ...
struct Recipe {
    std::string inputId;
    std::string outputId;
    std::string outputTexture;
    int durationTicks;
};

const std::vector<Recipe> furnaceRecipes = {
    {"raw_iron",    "iron_ingot",   "/Sprites/item/iron_ingot.png",  20}
};

std::map<std::string, sf::Texture> GridSystem::textureCache;

sf::Texture* GridSystem::getTexture(const std::string& name) {
    if (name.empty()) return nullptr;
    if (textureCache.find(name) == textureCache.end()) {
        sf::Texture tex;
        std::string fullPath = PATH + name;
        if (!tex.loadFromFile(fullPath)) {
            std::cerr << "Failed to load texture: " << fullPath << std::endl;
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
        if (tex) blockShape.setTexture(tex);
    }
}

void GameBlock::setRotation(int rot) {
    rotationState = rot % 4;
    blockShape.setRotation(sf::degrees(rotationState * 90.f)); 
}

GridSystem::GridSystem() {
    gridMatrix.resize(50, std::vector<GameBlock>(30)); 
}

void GridSystem::placeBlock(GameBlock &block, unsigned gridX, unsigned gridY) {
    if (gridX < gridMatrix.size() && gridY < gridMatrix[0].size()) {
        GameBlock newBlock = block; 
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
        gridMatrix[gridX][gridY].blockShape.setPosition({(float)gridX * 32 + 16, (float)gridY * 32 + 16});
        gridMatrix[targetX][targetY].blockShape.setPosition({(float)targetX * 32 + 16, (float)targetY * 32 + 16});
    }
}

void GridSystem::clear() {
    GameBlock emptyBlock;
    for (auto& row : gridMatrix) std::fill(row.begin(), row.end(), emptyBlock);
}

void GridSystem::render(sf::RenderWindow& window) {
    for (auto& row : gridMatrix) {
        for (auto& block : row) {
            if (!block.textureName.empty()) {
                if (block.blockShape.getTexture() == nullptr) {
                     sf::Texture* tex = GridSystem::getTexture(block.textureName);
                     if (tex) block.blockShape.setTexture(tex);
                }
                window.draw(block.blockShape);

                sf::Vector2f center = block.blockShape.getPosition(); 
                auto drawItem = [&](const Item& item, float offsetX, float offsetY, float scale = 0.5f) {
                    if (!item.texturePath.empty()) {
                        sf::Texture* t = GridSystem::getTexture(item.texturePath);
                        if (t) {
                            sf::RectangleShape s({32.f * scale, 32.f * scale});
                            s.setOrigin({(32.f * scale)/2, (32.f * scale)/2});
                            s.setPosition({center.x + offsetX, center.y + offsetY});
                            s.setTexture(t);
                            window.draw(s);
                        }
                    }
                };

                if (block.blockType == "furnace") {
                    if (block.inputInventory.itemCount > 0) drawItem(block.inputInventory.item, -8.f, -8.f, 0.4f);
                    if (block.outputInventory.itemCount > 0) drawItem(block.outputInventory.item, 8.f, -8.f, 0.4f);
                    if (block.fuelInventory.itemCount > 0) drawItem(block.fuelInventory.item, 0.f, 8.f, 0.4f);
                }
                else if (block.blockType == "hopper") {
                    if (block.inputInventory.itemCount > 0) drawItem(block.inputInventory.item, 0.f, 0.f, 0.5f);
                }
                else {
                    const Item* itemToShow = nullptr;
                    if (block.outputInventory.itemCount > 0) itemToShow = &block.outputInventory.item;
                    else if (block.inputInventory.itemCount > 0) itemToShow = &block.inputInventory.item;
                    if (itemToShow) drawItem(*itemToShow, 6.f, 6.f, 0.5f);
                }
            }
        }
    }
}

void GridSystem::updateGameLogic(unsigned long long currentTick, TaskManager& taskManager) {
    for (int x = 0; x < gridMatrix.size(); ++x) {
        for (int y = 0; y < gridMatrix[x].size(); ++y) {
            GameBlock& block = gridMatrix[x][y];

            // === 1. PICKAXE (БУР) ===
            if (block.blockType == "pickaxe") {
                int dx = 0, dy = 0;
                if (block.rotationState == 0) dy = 1; else if (block.rotationState == 1) dx = -1; else if (block.rotationState == 2) dy = -1; else if (block.rotationState == 3) dx = 1; 
                int tx = x + dx; int ty = y + dy;
                if (tx >= 0 && tx < gridMatrix.size() && ty >= 0 && ty < gridMatrix[0].size()) {
                    GameBlock& target = gridMatrix[tx][ty];
                    bool mineAction = false; std::string itemId = "", itemTex = "";
                    if (target.blockType == "iron_ore") { if (currentTick % 5 == 0) { itemId = "raw_iron"; itemTex = "/Sprites/item/raw_iron.png"; mineAction = true; } }
                    else if (target.blockType == "coal_ore") { if (currentTick % 2 == 0) { itemId = "coal"; itemTex = "/Sprites/item/coal.png"; mineAction = true; } }
                    if (mineAction && !itemId.empty() && block.outputInventory.itemCount < 64) block.outputInventory.addItem(Item(itemId, itemTex), 1);
                }
            }

            // === 2. FURNACE (ПЕЧЬ) ===
            if (block.blockType == "furnace") {
                bool hasInput = block.inputInventory.itemCount > 0;
                bool hasFuel  = block.fuelLevel > 0 || block.fuelInventory.itemCount > 0;
                if (hasInput && hasFuel) {
                    const Recipe* currentRecipe = nullptr;
                    for (const auto& r : furnaceRecipes) { if (r.inputId == block.inputInventory.item.itemId) { currentRecipe = &r; break; } }
                    if (currentRecipe) {
                        if (block.fuelLevel <= 0 && block.fuelInventory.itemCount > 0) { block.fuelInventory.removeItem(1); block.fuelLevel = 200.f; }
                        if (block.fuelLevel > 0) {
                            block.fuelLevel -= 1.0f; block.craftingProgress += 1;
                            if (block.craftingProgress >= currentRecipe->durationTicks) {
                                Item resultItem(currentRecipe->outputId, currentRecipe->outputTexture);
                                if (block.outputInventory.addItem(resultItem, 1)) { block.inputInventory.removeItem(1); block.craftingProgress = 0; }
                                else { block.craftingProgress = currentRecipe->durationTicks; }
                            }
                        } else { if (block.craftingProgress > 0) block.craftingProgress--; }
                    } else { block.craftingProgress = 0; }
                } else { block.craftingProgress = 0; }
            }

            // === 3. HOPPER (ВОРОНКА) ===
            if (block.blockType == "hopper") {
                if (currentTick % 5 == 0) { 
                    bool actionDone = false; 

                    // --- A. PUSH (ТОЛКАНИЕ ВПЕРЕД) ---
                    if (block.inputInventory.itemCount > 0) {
                        int frontX = x, frontY = y;
                        if (block.rotationState == 0) frontY = y + 1;      
                        else if (block.rotationState == 1) frontX = x - 1;
                        else if (block.rotationState == 2) frontY = y - 1;
                        else if (block.rotationState == 3) frontX = x + 1; 

                        if (frontX >= 0 && frontX < gridMatrix.size() && frontY >= 0 && frontY < gridMatrix[0].size()) {
                            GameBlock& target = gridMatrix[frontX][frontY];
                            
                            // ВАЖНАЯ ПРОВЕРКА: Толкаем только в валидные контейнеры!
                            bool isValidContainer = (target.blockType == "furnace" || 
                                                     target.blockType == "hopper" || 
                                                     target.blockType == "target" ||
                                                     target.blockType == "pickaxe"); // В бур тоже можно толкать (например, уголь, если добавим топливо)

                            if (isValidContainer) {
                                Item toMove = block.inputInventory.item;
                                bool success = false;
                                
                                if (target.blockType == "furnace") {
                                    if (toMove.itemId == "coal") { if (target.fuelInventory.addItem(toMove, 1)) success = true; } 
                                    else { if (target.inputInventory.addItem(toMove, 1)) success = true; }
                                }
                                else if (target.blockType == "target") {
                                    if (taskManager.isNeeded(toMove.itemId)) { if (target.inputInventory.addItem(toMove, 1)) success = true; }
                                }
                                else {
                                    if (target.inputInventory.addItem(toMove, 1)) success = true;
                                }

                                if (success) {
                                    block.inputInventory.removeItem(1);
                                    actionDone = true; 
                                }
                            }
                        }
                    }

                    // --- B. PULL (ВТЯГИВАНИЕ СЗАДИ) ---
                    if (!actionDone) {
                        int backX = x, backY = y;
                        if (block.rotationState == 0) backY = y - 1;      
                        else if (block.rotationState == 1) backX = x + 1;
                        else if (block.rotationState == 2) backY = y + 1;
                        else if (block.rotationState == 3) backX = x - 1;

                        if (backX >= 0 && backX < gridMatrix.size() && backY >= 0 && backY < gridMatrix[0].size()) {
                            GameBlock& source = gridMatrix[backX][backY];
                            
                            if (source.blockType == "furnace" || source.blockType == "pickaxe") {
                                if (source.outputInventory.itemCount > 0) {
                                    Item toMove = source.outputInventory.item;
                                    if (block.inputInventory.addItem(toMove, 1)) source.outputInventory.removeItem(1);
                                }
                            }
                            else if (source.blockType == "hopper") {
                                 if (source.inputInventory.itemCount > 0) {
                                     Item toMove = source.inputInventory.item;
                                     if (block.inputInventory.addItem(toMove, 1)) source.inputInventory.removeItem(1);
                                 }
                            }
                        }
                    }
                }
            }

            // === 4. TARGET ===
            if (block.blockType == "target") {
                if (block.inputInventory.itemCount > 0) {
                     if (taskManager.submitItem(block.inputInventory.item.itemId)) block.inputInventory.removeItem(1);
                }
            }
        }
    }
}
