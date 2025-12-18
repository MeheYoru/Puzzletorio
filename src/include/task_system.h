#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

struct GameTask {
    std::string itemId;       
    std::string displayName;  
    int requiredAmount;       
    int currentAmount;        
    bool isCompleted;

    GameTask(std::string id, std::string name, int req) 
        : itemId(id), displayName(name), requiredAmount(req), currentAmount(0), isCompleted(false) {}
};

class TaskManager {
public:
    std::vector<GameTask> tasks;
    sf::Font font;
    bool fontLoaded;

    TaskManager() {
        fontLoaded = false;
        // Попытка загрузить стандартный шрифт
        if (font.openFromFile("../../src/Fonts/Monocraft.ttc")) fontLoaded = true;
        else if (font.openFromFile("C:/Windows/Fonts/arial.ttf")) fontLoaded = true;
        
        addTask("iron_ingot", "Iron Ingot", 64);
        addTask("raw_iron", "Raw Iron", 32);
    }

    void addTask(std::string itemId, std::string name, int amount) {
        tasks.emplace_back(itemId, name, amount);
    }

    bool isNeeded(const std::string& itemId) {
        for (const auto& task : tasks) {
            if (task.itemId == itemId && !task.isCompleted) return true;
        }
        return false;
    }

    bool submitItem(const std::string& itemId) {
        for (auto& task : tasks) {
            if (!task.isCompleted && task.itemId == itemId) {
                task.currentAmount++;
                if (task.currentAmount >= task.requiredAmount) {
                    task.currentAmount = task.requiredAmount;
                    task.isCompleted = true;
                }
                return true; 
            }
        }
        return false; 
    }

    void save(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        for (const auto& task : tasks) {
            file << task.itemId << " " << task.currentAmount << " " << task.isCompleted << "\n";
        }
    }

    void load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;
        std::string id; int current; bool completed;
        while (file >> id >> current >> completed) {
            for (auto& task : tasks) {
                if (task.itemId == id) {
                    task.currentAmount = current;
                    task.isCompleted = completed;
                    break;
                }
            }
        }
    }

    void reset() {
        for (auto& task : tasks) {
            task.currentAmount = 0;
            task.isCompleted = false;
        }
    }

    void render(sf::RenderWindow& window) {
        if (!fontLoaded) return;
        sf::RectangleShape bg({320.f, 1080.f}); 
        bg.setPosition({1600.f, 0.f});
        bg.setFillColor(sf::Color(40, 40, 45));
        bg.setOutlineColor(sf::Color::Black);
        bg.setOutlineThickness(2.f);
        window.draw(bg);

        sf::Text title(font, "Current Goals:", 24); 
        title.setPosition({1620.f, 20.f});
        title.setFillColor(sf::Color::White);
        window.draw(title);

        float yPos = 70.f;
        for (const auto& task : tasks) {
            std::string status = task.displayName + " " + 
                                 std::to_string(task.currentAmount) + " / " + 
                                 std::to_string(task.requiredAmount);
            sf::Text taskText(font, status, 20);
            taskText.setPosition({1620.f, yPos});
            if (task.isCompleted) taskText.setFillColor(sf::Color::Green);
            else taskText.setFillColor(sf::Color::White);
            window.draw(taskText);
            yPos += 40.f;
        }
    }
};
