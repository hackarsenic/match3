#pragma once

#include "Gem.h"
#include "json.hpp"

using nlohmann::json;

class GameConfig
{
public:
    static GameConfig &GetInstance();

    [[nodiscard]] const std::filesystem::path &GetResourcePath() const { return g_pathToResources; }
    [[nodiscard]] constexpr unsigned GetCellSize() const { return g_cellSize; }
    [[nodiscard]] constexpr unsigned GetRowSize() const { return g_rowSize; }
    [[nodiscard]] constexpr unsigned GetColSize() const { return g_colSize; }
    [[nodiscard]] constexpr int GetMoveCount() const { return g_moveCount; }
    [[nodiscard]] std::map<int, int> GetObjectives() const { return g_objectives; }

    GameConfig(const GameConfig &rhs) = delete;
    GameConfig &operator=(const GameConfig &rhs) = delete;

private:
    GameConfig() = default;
    void InitConfig();

private:
    static bool g_isConfigLoaded;

    const std::filesystem::path g_pathToResources = "./resources/";
    const unsigned g_cellSize = 75;
    unsigned g_rowSize = 5;
    unsigned g_colSize = 5;
    int g_moveCount = -1;
    std::map<int, int> g_objectives;
};
