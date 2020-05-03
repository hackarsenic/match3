#pragma once

#include <SFML/Graphics.hpp>
#include <Map>

//Singlton resource manager class
class ResourceManager
{
private:
	ResourceManager() = default;
	std::map<std::string, sf::Texture> _textures;
	//static ResourceManager *instance;
	
public:
	static ResourceManager& get_instance();
	static sf::Texture& GetTexture(const std::string &filename);

	//prohibit copy and assign of singleton
	ResourceManager(ResourceManager const&) = delete;
	void operator=(ResourceManager const&) = delete;
};