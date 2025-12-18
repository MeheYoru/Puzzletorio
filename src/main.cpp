#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cstdio> 
#include "./include/grid_system.h"
#include "./include/game_saver.h"
#include "./include/task_system.h"

#define PATH "../../src" 
#define SPRITE(name) PATH "/Sprites/" name ".png"

enum directions { down, right, up, left };
enum windowstate { WINDOWED, FULLSCREEN };

bool WindowState = FULLSCREEN;

int main() {
    unsigned int width = 1920;
    unsigned int height = 1080;
    
    sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();
    sf::VideoMode windowedMode = sf::VideoMode({width, height});

    sf::RenderWindow window(
        (WindowState == FULLSCREEN ? fullscreenMode : windowedMode), 
        "Puzzletorio",
        (WindowState == FULLSCREEN ? sf::State::Fullscreen : sf::State::Windowed)
    );
    window.setFramerateLimit(60);

    sf::Texture tileTexture;
    if (!tileTexture.loadFromFile(SPRITE("tile_32px"))) return -1;
    tileTexture.setRepeated(true);
    sf::RectangleShape tiledArea({1920.f, 960.f});
    tiledArea.setTexture(&tileTexture);
    tiledArea.setTextureRect(sf::IntRect({{0, 0}, {1920, 960}}));

    sf::Texture tileHighlightedTexture;
    if (!tileHighlightedTexture.loadFromFile(SPRITE("tile_highlighted_32px"))) return -1;
    sf::RectangleShape tileHighlighted({32.f, 32.f});
    tileHighlighted.setTexture(&tileHighlightedTexture);
    tileHighlighted.setTextureRect(sf::IntRect({{0, 0}, {32, 32}}));
    tileHighlighted.setFillColor(sf::Color(0x00000044));

    sf::RectangleShape toolbarArea({1920.f, 120.f});
    toolbarArea.setFillColor(sf::Color(30, 30, 30));
    toolbarArea.setPosition({0.f, 960.f});

    sf::RectangleShape selectionBox({40.f, 40.f});
    selectionBox.setFillColor(sf::Color::Transparent);
    selectionBox.setOutlineColor(sf::Color::Yellow);
    selectionBox.setOutlineThickness(3.f);

    GridSystem worldGrid;
    TaskManager taskManager;

    std::vector<GameBlock> hotbarItems;
    hotbarItems.emplace_back("furnace", "/block/furnace_front_on.png");
    hotbarItems.emplace_back("crafter", "/block/crafter_top_crafting.png");
    hotbarItems.emplace_back("hopper", "/item/hopper.png");
    hotbarItems.emplace_back("pickaxe", "/item/diamond_pickaxe.png");
    hotbarItems.emplace_back("iron_ore", "/block/iron_ore.png"); 
    hotbarItems.emplace_back("coal_ore", "/block/coal_ore.png");

    int selectedSlot = -1;
    float slotSize = 80.f;
    float startX = (width - (hotbarItems.size() * slotSize)) / 2.0f;

    GameBlock ghostBlock; 

    // === ЗАГРУЗКА ===
    GameSaver::load(worldGrid, "autosave.dat");
    taskManager.load("tasks.dat");

    if (worldGrid.gridMatrix[30][15].blockType.empty()) {
        GameBlock targetBlock("target", "/block/target_top.png");
        worldGrid.placeBlock(targetBlock, 30, 15);
    }

    bool ctrlEnterPressed = false;
    GameBlock* draggedBlock = nullptr;
    GameBlock tempBlock;

    sf::Clock tickClock;
    const sf::Time tickRate = sf::seconds(1.f / 20.f); 
    unsigned long long globalTicks = 0;

    while (window.isOpen()) {
        if (tickClock.getElapsedTime() >= tickRate) {
            worldGrid.updateGameLogic(globalTicks, taskManager);
            globalTicks++;
            tickClock.restart();
        }
        
        bool altDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RAlt);
        bool shiftDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RShift);
        bool enterDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Enter);

        // === ПОЛНЫЙ СБРОС (SHIFT + R) ===
        if (shiftDown && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::R)) {
            worldGrid.clear();
            taskManager.reset();
            
            GameBlock targetBlock("target", "/block/target_top.png");
            worldGrid.placeBlock(targetBlock, 30, 15);

            std::remove("autosave.dat");
            std::remove("tasks.dat");

            draggedBlock = nullptr;
            selectedSlot = -1;
            std::cout << "GAME RESET COMPLETE" << std::endl;
        }

        if (altDown && enterDown && !ctrlEnterPressed) {
            ctrlEnterPressed = true;
            window.close();
            if (WindowState == WINDOWED) {
                window.create(fullscreenMode, "Puzzletorio", sf::State::Fullscreen);
                WindowState = FULLSCREEN;
            } else {
                window.create(windowedMode, "Puzzletorio", sf::State::Windowed);
                WindowState = WINDOWED;
            }
            window.setFramerateLimit(60);
        }
        if (!altDown || !enterDown) ctrlEnterPressed = false;

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        unsigned gridX = mousePos.x / 32;
        unsigned gridY = mousePos.y / 32;

        auto pos32 = sf::Vector2f((float)gridX * 32, (float)gridY * 32);
        bool isMouseInGameplayArea = (mousePos.x < 1600 && pos32.y < 960);

        if (isMouseInGameplayArea) {
            tileHighlighted.setPosition(pos32);
            ghostBlock.blockShape.setPosition({pos32.x + 16.f, pos32.y + 16.f});
        } else {
            tileHighlighted.setPosition({-100, -100});
        }

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } 
            else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }
                
                // Проверка !shiftDown, чтобы R не срабатывало при ресете
                if (keyEvent->scancode == sf::Keyboard::Scancode::R && !shiftDown) { 
                    if (selectedSlot != -1) {
                        if (hotbarItems[selectedSlot].blockType != "furnace") {
                            int nextRot = ghostBlock.rotationState + 1;
                            ghostBlock.setRotation(nextRot);
                            hotbarItems[selectedSlot].setRotation(nextRot);
                        }
                    }
                    else if (isMouseInGameplayArea && gridX < worldGrid.gridMatrix.size() && gridY < worldGrid.gridMatrix[0].size()) {
                        GameBlock& target = worldGrid.gridMatrix[gridX][gridY];
                        if (!target.textureName.empty()) {
                            if (target.blockType != "furnace" && target.blockType != "target") {
                                int nextRot = target.rotationState + 1;
                                target.setRotation(nextRot);
                            }
                        }
                    }
                }
                
                int keyNum = -1;
                switch (keyEvent->scancode) {
                    case sf::Keyboard::Scancode::Num1: keyNum = 0; break;
                    case sf::Keyboard::Scancode::Num2: keyNum = 1; break;
                    case sf::Keyboard::Scancode::Num3: keyNum = 2; break;
                    case sf::Keyboard::Scancode::Num4: keyNum = 3; break;
                    case sf::Keyboard::Scancode::Num5: keyNum = 4; break;
                    case sf::Keyboard::Scancode::Num6: keyNum = 5; break;
                    default: break;
                }
                if (keyNum != -1 && keyNum < hotbarItems.size()) {
                    selectedSlot = keyNum;
                    ghostBlock = hotbarItems[selectedSlot];
                    ghostBlock.blockShape.setFillColor(sf::Color(255, 255, 255, 150));
                }
            }
            else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                 if (selectedSlot == -1) { selectedSlot = 0; } 
                 else {
                    if (wheel->delta > 0) { selectedSlot--; if (selectedSlot < 0) selectedSlot = hotbarItems.size() - 1; } 
                    else if (wheel->delta < 0) { selectedSlot++; if (selectedSlot >= hotbarItems.size()) selectedSlot = 0; }
                }
                ghostBlock = hotbarItems[selectedSlot];
                ghostBlock.blockShape.setFillColor(sf::Color(255, 255, 255, 150));
            }
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePos.y >= 960) {
                    if (mousePressed->button == sf::Mouse::Button::Left) {
                        float relX = (float)mousePos.x - startX;
                        if (relX >= 0) {
                            int clickedIndex = (int)(relX / slotSize);
                            if (clickedIndex >= 0 && clickedIndex < hotbarItems.size()) {
                                selectedSlot = clickedIndex;
                                ghostBlock = hotbarItems[selectedSlot];
                                ghostBlock.blockShape.setFillColor(sf::Color(255, 255, 255, 150));
                            }
                        }
                    }
                }
                else if (isMouseInGameplayArea) {
                    if (gridX < worldGrid.gridMatrix.size() && gridY < worldGrid.gridMatrix[0].size()) {
                        if (worldGrid.gridMatrix[gridX][gridY].blockType == "target") { /* Ничего */ }
                        else if (mousePressed->button == sf::Mouse::Button::Left) {
                            if (worldGrid.gridMatrix[gridX][gridY].textureName.empty()) {
                                if (selectedSlot != -1) {
                                    worldGrid.placeBlock(hotbarItems[selectedSlot], gridX, gridY);
                                    selectedSlot = -1; 
                                }
                            } 
                            else {
                                draggedBlock = &(worldGrid.gridMatrix[gridX][gridY]);
                                tempBlock = *draggedBlock;
                            }
                        }
                        else if (mousePressed->button == sf::Mouse::Button::Middle) {
                            GameBlock emptyBlock;
                            worldGrid.placeBlock(emptyBlock, gridX, gridY);
                        }
                    }
                }
            } 
            else if (const auto* mouseRelease = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseRelease->button == sf::Mouse::Button::Left) {
                    if (draggedBlock && draggedBlock->textureName != "") { // Проверяем имя, а не getSize
                         if (mousePos.x < 1600 && mousePos.y < 960) {
                            worldGrid.swapBlocks(draggedBlock->blockShape.getPosition().x / 32, 
                                                 draggedBlock->blockShape.getPosition().y / 32, mousePos);
                         }
                    }
                    draggedBlock = nullptr;
                }
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && draggedBlock) {
            tempBlock.blockShape.setPosition({(float)mousePos.x, (float)mousePos.y});
        }

        window.clear(sf::Color(0x202020FF));
        
        window.draw(tiledArea);
        window.draw(toolbarArea);
        
        worldGrid.render(window);
        taskManager.render(window);

        for (int i = 0; i < hotbarItems.size(); i++) {
            GameBlock& item = hotbarItems[i];
            float yPos = 960.f + (120.f - 32.f) / 2.f;
            item.blockShape.setPosition({startX + i * slotSize + 24 + 16.f, yPos + 16.f});
            window.draw(item.blockShape);
            if (i == selectedSlot) {
                selectionBox.setPosition({startX + i * slotSize + 24 - 4, yPos - 4});
                window.draw(selectionBox);
            }
        }
        
        if (!draggedBlock && isMouseInGameplayArea && selectedSlot != -1) {
            window.draw(ghostBlock.blockShape);
        }

        if (isMouseInGameplayArea) window.draw(tileHighlighted);
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && draggedBlock) {
             window.draw(tempBlock.blockShape);
        }
        
        tempBlock.blockShape.setPosition({-32, -32});
        window.display();
    }

    GameSaver::save(worldGrid, "autosave.dat");
    taskManager.save("tasks.dat");

    return 0;
}
