#pragma once

#include <SFML/Graphics.hpp>
#include <map>

// Singlton resource manager class
class ResourceManager
{
private:
    ResourceManager() = default;
    std::map<std::string, sf::Texture> r_textures;

public:
    static ResourceManager &GetInstance();
    static sf::Texture &GetTexture(const std::string &filename);

    // prohibit copy and assign of singleton
    ResourceManager(ResourceManager const &) = delete;
    void operator=(ResourceManager const &) = delete;
};