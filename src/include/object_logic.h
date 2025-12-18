#pragma once
#include <string>
#include "./grid_system.h"
#include "./task_system.h"

class BlockLogic
{
public:
    virtual ~BlockLogic() = default;
    virtual void update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager) {}
};

class HopperLogic : public BlockLogic
{
public:
    void update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager) override;
};

class PickaxeLogic : public BlockLogic
{
public:
    void update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager) override;
};

class FurnaceLogic : public BlockLogic
{
public:
    void update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager) override;
};

class TargetLogic : public BlockLogic
{
public:
    void update(int gridX, int gridY, GridSystem &gridSystem, unsigned long long currentTick, TaskManager &taskManager) override;
};

class LogicFactory
{
public:
    static BlockLogic *createLogic(const std::string &type);
};
