#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>

#include <SFML/Graphics.hpp>

class Animation;
class Script;

class Assets
{
	/**
	 * @brief Map of texture names to their corresponding textures
	 */
	std::map<std::string, sf::Texture> m_textureMap;
	/**
	 * @brief Map of animation names to their corresponding animations
	 */
	std::map<std::string, Animation> m_animationMap;
	/**
	 * @brief Map of font names to their corresponding fonts
	 */
	std::map<std::string, sf::Font> m_fontMap;
	/**
	 * @brief Map of script names to their corresponding script file paths
	 */
	std::map<std::string, Script> m_scriptMap;

	/**
	 * @brief Add a texture to the asset manager
	 *
	 * @param textureName The name of the texture
	 * @param path The file path to load the texture from
	 * @param smooth Whether to apply smoothing to the texture
	 */
	void addTexture(const std::string& textureName, const std::string& path, bool smooth = true);
	/**
	 * @brief Add an animation to the asset manager
	 *
	 * @param animationName The name of the animation
	 * @param textureName The name of the texture to get frames from
	 * @param frames The total number of frames in the animation
	 * @param speed The speed to play the animation
	 */
	void addAnimation(const std::string& animationName, const std::string& textureName, const size_t& frames, const size_t& speed);
	/**
	 * @brief Add a font to the asset manager
	 *
	 * @param fontName The name of the font
	 * @param path The file path to load the font from
	 */
	void addFont(const std::string& fontName, const std::string& path);
	/**
	 * @brief Add a script to the asset manager
	 *
	 * @param scriptName The name of the script
	 * @param path The file path to load the script from
	 */
	void addScript(const std::string& scriptName, const std::string& path);

	/**
	 * @brief Default constructor
	 */
	Assets() = default;

public:
	/**
	 * @brief Get the singleton instance of the Assets manager
	 *
	 * @return Reference to the Assets instance
	 */
	static Assets& Instance();

	/**
	 * @brief Reload the content of the scripts
	 */
	void reloadScripts();

	/**
	 * @brief Load assets from a configuration file
	 *
	 * @param path The path to the configuration file
	 */
	void loadFromFile(const std::string& path, const std::string& base = "");

	/**
	 * @brief Get a texture by its name
	 *
	 * @param textureName The name of the texture
	 * @return Reference to the requested texture
	 * @throws std::runtime_error
	 */
	const sf::Texture& getTexture(const std::string& textureName) const;

	/**
	 * @brief Get an animation by its name
	 *
	 * @param animationName The name of the animation
	 * @return Reference to the requested animation
	 * @throws std::runtime_error
	 */
	const Animation& getAnimation(const std::string& animationName) const;

	/**
	 * @brief Get a font by its name
	 *
	 * @param fontName The name of the font
	 * @return Reference to the requested font
	 * @throws std::runtime_error
	 */
	const sf::Font& getFont(const std::string& fontName) const;

	/**
	 * @brief Get a script by its name
	 *
	 * @param scriptName The name of the script
	 * @return Reference to the requested script
	 * @throws std::runtime_error
	 */
	const Script& getScript(const std::string& scriptName) const;

	/**
	 * @brief Get all loaded textures
	 *
	 * @return Map of texture names to their corresponding textures
	 */
	const std::map<std::string, sf::Texture>& getTextures() const;

	/**
	 * @brief Get all loaded animations
	 *
	 * @return Map of animation names to their corresponding animations
	 */
	std::map<std::string, Animation>& getAnimations();

	/**
	 * @brief Get all loaded fonts
	 *
	 * @return Map of animation names to their corresponding fonts
	 */
	std::map<std::string, sf::Font>& getFonts();

	/**
	 * @brief Get all loaded scripts
	 *
	 * @return Map of script names to their corresponding script objectss
	 */
	std::map<std::string, Script>& getScripts();
};
