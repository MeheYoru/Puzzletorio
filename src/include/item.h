#pragma once
#include <string>
#include <iostream>

class Item {
public:
    std::string itemId;       
    std::string texturePath;  

    Item(std::string id = "", std::string path = "") 
        : itemId(id), texturePath(path) {}

    Item(const Item& other) {
        this->itemId = other.itemId;
        this->texturePath = other.texturePath;
    }

    Item& operator=(const Item& other) {
        if (this != &other) {
            this->itemId = other.itemId;
            this->texturePath = other.texturePath;
        }
        return *this;
    }

    bool operator==(const Item& other) const {
        return itemId == other.itemId;
    }
    
    bool isEmpty() const {
        return itemId.empty();
    }
};

class Inventory {
public:
    Item item;       
    int itemCount;       
    const int MAX_STACK_SIZE = 64;

    Inventory() : itemCount(0) {}

    Inventory(const Inventory& other) {
        this->item = other.item;
        this->itemCount = other.itemCount;
    }

    Inventory& operator=(const Inventory& other) {
        if (this != &other) {
            this->item = other.item;
            this->itemCount = other.itemCount;
        }
        return *this;
    }

    bool addItem(Item newItem, int amount = 1) {
        if (amount <= 0) return false;

        if (itemCount == 0 || item.isEmpty()) {
            if (amount > MAX_STACK_SIZE) return false;
            item = newItem;
            itemCount = amount;
            return true;
        }

        if (!(item == newItem)) {
            return false;
        }

        if (itemCount + amount > MAX_STACK_SIZE) {
            return false;
        }

        itemCount += amount;
        return true;
    }

    bool removeItem(int amount = 1) {
        if (itemCount < amount) return false;
        
        itemCount -= amount;
        if (itemCount == 0) {
            item = Item(); 
        }
        return true;
    }
    
    void clear() {
        itemCount = 0;
        item = Item();
    }
};
