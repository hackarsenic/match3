#pragma once

#include <fstream>

#include "Gem.h"
#include "json.hpp"
#include "resources.h"

using nlohmann::json;

class GameConfig
{
public:
    static GameConfig &GetInstance();

    constexpr uint16_t GetCellSize() const { return g_cellSize; }
    constexpr uint16_t GetRowSize() const { return g_rowSize; }
    constexpr uint16_t GetColSize() const { return g_colSize; }
    constexpr int16_t GetMoveCount() const { return g_moveCount; }
    std::map<int, int> GetObjectives() const { return g_objectives; }

    GameConfig(const GameConfig &rhs) = delete;
    GameConfig &operator=(const GameConfig &rhs) = delete;

private:
    GameConfig() = default;
    void InitConfig();

private:
    static bool g_isConfigLoaded;

    const uint16_t g_cellSize = 75;
    uint16_t g_rowSize = 5;
    uint16_t g_colSize = 5;
    int16_t g_moveCount = -1;
    std::map<int, int> g_objectives;
};

