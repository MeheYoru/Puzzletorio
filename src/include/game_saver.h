#pragma once
#include <fstream>
#include <string>
#include "./grid_system.h"

class GameSaver
{
public:
    static void save(const GridSystem &gridSystem, const std::string &filename)
    {
        std::ofstream file(filename);
        if (!file.is_open())
            return;

        for (int x = 0; x < gridSystem.gridMatrix.size(); ++x)
        {
            for (int y = 0; y < gridSystem.gridMatrix[x].size(); ++y)
            {
                const auto &block = gridSystem.gridMatrix[x][y];
                if (!block.textureName.empty())
                {
                    file << x << " " << y << " "
                         << block.blockType << " "
                         << block.textureName << " "
                         << block.rotationState << " "
                         << block.craftingProgress << " "
                         << block.fuelLevel << " ";

                    auto saveInv = [&](const Inventory &inv)
                    {
                        if (inv.itemCount > 0 && !inv.item.isEmpty())
                        {
                            file << inv.item.itemId << " " << inv.item.texturePath << " " << inv.itemCount << " ";
                        }
                        else
                        {
                            file << "empty_slot" << " " << "none" << " " << 0 << " ";
                        }
                    };
                    saveInv(block.inputInventory);
                    saveInv(block.outputInventory);
                    saveInv(block.fuelInventory);
                    file << "\n";
                }
            }
        }
    }

    static void load(GridSystem &gridSystem, const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
            return;

        unsigned int x, y;
        std::string type, texName;
        int rotation, progress;
        float fuel;

        std::string id1, tex1;
        int cnt1;
        std::string id2, tex2;
        int cnt2;
        std::string id3, tex3;
        int cnt3;

        while (file >> x >> y >> type >> texName >> rotation >> progress >> fuel >> id1 >> tex1 >> cnt1 >> id2 >> tex2 >> cnt2 >> id3 >> tex3 >> cnt3)
        {

            if (x < gridSystem.gridMatrix.size() && y < gridSystem.gridMatrix[0].size())
            {
                GameBlock loadedBlock(type, texName);
                loadedBlock.setRotation(rotation);
                loadedBlock.craftingProgress = progress;
                loadedBlock.fuelLevel = fuel;

                if (id1 != "empty_slot")
                    loadedBlock.inputInventory.addItem(Item(id1, tex1), cnt1);
                if (id2 != "empty_slot")
                    loadedBlock.outputInventory.addItem(Item(id2, tex2), cnt2);
                if (id3 != "empty_slot")
                    loadedBlock.fuelInventory.addItem(Item(id3, tex3), cnt3);

                gridSystem.placeBlock(loadedBlock, x, y);
            }
        }
    }
};
