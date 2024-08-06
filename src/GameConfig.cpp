#include <fstream>

#include "GameConfig.h"

bool GameConfig::g_isConfigLoaded = false;

GameConfig &GameConfig::GetInstance()
{
    static GameConfig config;
    if (!g_isConfigLoaded) {
        config.InitConfig();
    }

    return config;
}

void GameConfig::InitConfig()
{
    std::ifstream config_file(g_pathToResources / "config.json", std::ifstream::in);
    if (config_file.good()) {
        json config = json::parse(config_file);

        g_colSize = config["col_size"];
        g_rowSize = config["row_size"];
        g_moveCount = config["moves count"];

        for (auto it = config["objectives"].begin(); it != config["objectives"].end(); ++it) {
            if (it.key() == "red")
                g_objectives.emplace(static_cast<int>(Gem::Color::Red), it.value());
            else if (it.key() == "green")
                g_objectives.emplace(static_cast<int>(Gem::Color::Green), it.value());
            else if (it.key() == "blue")
                g_objectives.emplace(static_cast<int>(Gem::Color::Blue), it.value());
            else if (it.key() == "orange")
                g_objectives.emplace(static_cast<int>(Gem::Color::Orange), it.value());
            else if (it.key() == "violet")
                g_objectives.emplace(static_cast<int>(Gem::Color::Violet), it.value());
        }

        g_isConfigLoaded = true;
    }
    config_file.close();
}