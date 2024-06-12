#include "ResourceManager.h"

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}

sf::Texture &ResourceManager::GetTexture(const std::string &filename)
{
    auto &texture_map = GetInstance().r_textures;
    auto tex_found = texture_map.find(filename);

    if (tex_found != texture_map.end()) {
        tex_found->second.setSmooth(true);
        return tex_found->second;
    } else {
        sf::Texture &new_texture = texture_map[filename];
        new_texture.loadFromFile(filename);
        new_texture.setSmooth(true);
        return new_texture;
    }
}
