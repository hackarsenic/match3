#include "ResourceManager.hpp"

ResourceManager& ResourceManager::get_instance()
{
	static ResourceManager instance;
	return instance;
}

sf::Texture& ResourceManager::GetTexture(const std::string& filename)
{
	auto &texture_map = get_instance()._textures;
	auto tex_found = texture_map.find(filename);

	if(tex_found != texture_map.end())
	{
		tex_found->second.setSmooth(true);
		return tex_found->second;
	}
	else
	{
		sf::Texture &new_texture = texture_map[filename];
		new_texture.loadFromFile(filename);
		new_texture.setSmooth(true);
		return new_texture;
	}
}
