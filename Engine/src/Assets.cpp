#include "Assets.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Animation.hpp"
#include "INIReader.h"

Assets& Assets::Instance() {
	static Assets assets;
	return assets;
}

// -------------------------------------------------------
// Texture
// uses explicit Texture(path) constructor in SFML�3.0 :contentReference[oaicite:0]{index=0}
void Assets::addTexture(const std::string& textureName, const std::string& path, bool smooth) {
	m_textureMap[textureName] = sf::Texture();

	if (!m_textureMap[textureName].loadFromFile(path)) {
		std::cerr << "Could not load texture file: " << path << std::endl;
		m_textureMap.erase(textureName);
	}
	else {
		m_textureMap[textureName].setSmooth(smooth);
		std::cout << "Loaded Texture: " << path << std::endl;
	}
}

// -------------------------------------------------------
// Animation
void Assets::addAnimation(const std::string& animationName, const std::string& textureName, const size_t& frames, const size_t& speed) {
	m_animationMap[animationName] = Animation(animationName, textureName, frames, speed);
}

// -------------------------------------------------------
// Font
// uses explicit Font(path) constructor in SFML�3.0 :contentReference[oaicite:1]{index=1}
void Assets::addFont(const std::string& fontName, const std::string& path) {
	m_fontMap[fontName] = sf::Font();

	if (!m_fontMap[fontName].openFromFile(path)) {
		std::cerr << "Could not load font file: " << path << std::endl;
		m_fontMap.erase(fontName);
	}
	else {
		std::cout << "Loaded Font: " << path << std::endl;
	}
}

void Assets::loadFromFile(const std::string& config) {
	INIReader reader(config);

	for (auto section : reader.Sections()) {
		if (section.starts_with("texture.")) {
			auto name = section.substr(8, section.size());
			auto path = reader.GetString(section, "file", "");
			auto smooth = reader.GetBoolean(section, "smooth", false);
			addTexture(name, path, smooth);
		}
		else if (section.starts_with("animation.")) {
			auto name = section.substr(10, section.size());
			auto texture = reader.GetString(section, "texture", "");
			auto frames = reader.GetInteger(section, "frames", 1);
			auto speed = reader.GetInteger(section, "speed", 0);
			addAnimation(name, texture, frames, speed);
		}
		else if (section.starts_with("font.")) {
			auto name = section.substr(5, section.size());
			auto path = reader.GetString(section, "file", "");
			addFont(name, path);
		}
	}
}

// -------------------------------------------------------
// Getters
const sf::Texture& Assets::getTexture(const std::string& name) const {
	auto it = m_textureMap.find(name);
	std::cout << "Looking for texture: " << name << std::endl;
	assert(it != m_textureMap.end());
	return it->second;
}

const Animation& Assets::getAnimation(const std::string& name) const {
	auto it = m_animationMap.find(name);
	assert(it != m_animationMap.end());
	return it->second;
}

const sf::Font& Assets::getFont(const std::string& name) const {
	auto it = m_fontMap.find(name);
	assert(it != m_fontMap.end());
	return it->second;
}

const std::map<std::string, sf::Texture>& Assets::getTextures() const {
	return m_textureMap;
}

std::map<std::string, Animation>& Assets::getAnimations() {
	return m_animationMap;
}
