#include "./include/object_logic.h"
#include "./include/item.h"
#include <map>

struct CraftingRecipe
{
    std::string inputItemId;
    std::string outputItemId;
    std::string outputTexture;
    int durationTicks;
};

const std::vector<CraftingRecipe> RECIPES = {
    {"raw_iron", "iron_ingot", "/item/iron_ingot.png", 5}};

const CraftingRecipe *findRecipe(const std::string &inputId)
{
    for (const auto &r : RECIPES)
    {
        if (r.inputItemId == inputId)
            return &r;
    }
    return nullptr;
}

void calculateDirection(int rotationState, int &offsetX, int &offsetY)
{
    offsetX = 0;
    offsetY = 0;
    if (rotationState == 0)
        offsetY = -1;
    else if (rotationState == 1)
        offsetX = 1;
    else if (rotationState == 2)
        offsetY = 1;
    else if (rotationState == 3)
        offsetX = -1;
}

// === TARGET LOGIC ===
void TargetLogic::update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager)
{
    GameBlock &target = gridSystem.gridMatrix[gridX][gridY];

    if (target.inputInventory.itemCount > 0)
    {
        std::string itemId = target.inputInventory.item.itemId;
        bool accepted = taskManager.submitItem(itemId);

        if (accepted)
        {
            target.inputInventory.removeItem(1);
        }
    }
}

// === FURNACE LOGIC ===
void FurnaceLogic::update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager)
{
    GameBlock &furnace = gridSystem.gridMatrix[gridX][gridY];

    // Refuel
    if (furnace.fuelLevel <= 0 && furnace.fuelInventory.itemCount > 0)
    {
        if (furnace.fuelInventory.item.itemId == "coal")
        {
            furnace.fuelInventory.removeItem(1);
            furnace.fuelLevel += 200;
        }
    }
    if (furnace.fuelLevel <= 0)
    {
        furnace.craftingProgress = 0;
        return;
    }

    // Smelt
    if (furnace.inputInventory.itemCount > 0)
    {
        const CraftingRecipe *recipe = findRecipe(furnace.inputInventory.item.itemId);
        if (recipe)
        {
            Item resultItem(recipe->outputItemId, recipe->outputTexture);
            bool canOutput = false;
            if (furnace.outputInventory.itemCount == 0)
                canOutput = true;
            else if (furnace.outputInventory.item == resultItem && furnace.outputInventory.itemCount < 64)
                canOutput = true;

            if (canOutput)
            {
                furnace.craftingProgress++;
                furnace.fuelLevel--;
                if (furnace.craftingProgress >= recipe->durationTicks)
                {
                    furnace.inputInventory.removeItem(1);
                    furnace.outputInventory.addItem(resultItem, 1);
                    furnace.craftingProgress = 0;
                }
            }
            else
                furnace.craftingProgress = 0;
        }
        else
            furnace.craftingProgress = 0;
    }
    else
        furnace.craftingProgress = 0;
}

// === HOPPER LOGIC ===
void HopperLogic::update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager)
{
    int rot = gridSystem.gridMatrix[gridX][gridY].rotationState;
    int inputOffsetX, inputOffsetY;
    calculateDirection(rot, inputOffsetX, inputOffsetY);
    int outputOffsetX, outputOffsetY;
    calculateDirection((rot + 2) % 4, outputOffsetX, outputOffsetY);

    int inputX = gridX + inputOffsetX;
    int inputY = gridY + inputOffsetY;
    int outputX = gridX + outputOffsetX;
    int outputY = gridY + outputOffsetY;

    // EXTRACT
    if (gridSystem.gridMatrix[gridX][gridY].inputInventory.itemCount < 64)
    {
        if (inputX >= 0 && inputX < (int)gridSystem.gridMatrix.size() && inputY >= 0 && inputY < (int)gridSystem.gridMatrix[0].size())
        {
            GameBlock &sourceBlock = gridSystem.gridMatrix[inputX][inputY];
            Inventory *targetSourceInv = nullptr;
            if (sourceBlock.blockType == "furnace" || sourceBlock.blockType == "crafter")
                targetSourceInv = &sourceBlock.outputInventory;
            else
                targetSourceInv = &sourceBlock.inputInventory;

            if (targetSourceInv && targetSourceInv->itemCount > 0 && !targetSourceInv->item.isEmpty())
            {
                if (gridSystem.gridMatrix[gridX][gridY].inputInventory.itemCount == 0 ||
                    gridSystem.gridMatrix[gridX][gridY].inputInventory.item == targetSourceInv->item)
                {
                    Item takenItem = targetSourceInv->item;
                    if (targetSourceInv->removeItem(1))
                    {
                        gridSystem.gridMatrix[gridX][gridY].inputInventory.addItem(takenItem, 1);
                    }
                }
            }
        }
    }
    // INSERT
    if (gridSystem.gridMatrix[gridX][gridY].inputInventory.itemCount > 0)
    {
        if (outputX >= 0 && outputX < (int)gridSystem.gridMatrix.size() && outputY >= 0 && outputY < (int)gridSystem.gridMatrix[0].size())
        {
            GameBlock &targetBlock = gridSystem.gridMatrix[outputX][outputY];
            if (!targetBlock.blockType.empty())
            {
                Item itemToInsert = gridSystem.gridMatrix[gridX][gridY].inputInventory.item;
                if (targetBlock.blockType == "furnace" && itemToInsert.itemId == "coal")
                {
                    if (targetBlock.fuelInventory.addItem(itemToInsert, 1))
                    {
                        gridSystem.gridMatrix[gridX][gridY].inputInventory.removeItem(1);
                    }
                }
                else
                {
                    if (targetBlock.inputInventory.addItem(itemToInsert, 1))
                    {
                        gridSystem.gridMatrix[gridX][gridY].inputInventory.removeItem(1);
                    }
                }
            }
        }
    }
}

// === PICKAXE LOGIC ===
void PickaxeLogic::update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager)
{
    if (currentTick % 5 != 0)
        return;
    int rot = gridSystem.gridMatrix[gridX][gridY].rotationState;
    int offsetX, offsetY;
    calculateDirection((rot + 1) % 4, offsetX, offsetY);
    int targetX = gridX + offsetX;
    int targetY = gridY + offsetY;

    if (targetX >= 0 && targetX < (int)gridSystem.gridMatrix.size() && targetY >= 0 && targetY < (int)gridSystem.gridMatrix[0].size())
    {
        GameBlock &targetBlock = gridSystem.gridMatrix[targetX][targetY];
        if (targetBlock.blockType == "iron_ore")
        {
            Item ore("raw_iron", "/item/raw_iron.png");
            gridSystem.gridMatrix[gridX][gridY].inputInventory.addItem(ore, 1);
        }
        else if (targetBlock.blockType == "coal_ore")
        {
            if (currentTick % 2 == 0)
            {
                Item coal("coal", "/item/coal.png");
                gridSystem.gridMatrix[gridX][gridY].inputInventory.addItem(coal, 1);
            }
        }
    }
}

BlockLogic *LogicFactory::createLogic(const std::string &type)
{
    if (type == "target")
        return new TargetLogic();
    if (type == "furnace")
        return new FurnaceLogic();
    if (type == "hopper")
        return new HopperLogic();
    if (type == "pickaxe")
        return new PickaxeLogic();
    return new BlockLogic();
}
