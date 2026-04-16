#include "Assets.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>

#include "INIReader.h"

#include "Animation.hpp"
#include "Script.hpp"

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
		std::cerr << "[ERROR][ASSETS] Could not load texture file: " << path << std::endl;
		m_textureMap.erase(textureName);
	}
	else {
		m_textureMap[textureName].setSmooth(smooth);
		std::cout << "[INFO][ASSETS] Loaded Texture: [" << textureName << "] = " << path << std::endl;
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
		std::cerr << "[ERROR][ASSETS] Could not load font file: " << path << std::endl;
		m_fontMap.erase(fontName);
	}
	else {
		std::cout << "[INFO][ASSETS] Loaded Font: [" << fontName << "] = " << path << std::endl;
	}
}

// -------------------------------------------------------
// Script
// Note: This function only stores the path to the script, it does not load the script itself
void Assets::addScript(const std::string& scriptName, const std::string& path) {
	if (!std::filesystem::exists(path)) {
		std::cerr << "[ERROR][ASSETS] Script file not found: " << path << std::endl;
		return;
	}

	std::ifstream scriptFile(path);
	if (!scriptFile.is_open()) {
		std::cerr << "[ERROR][ASSETS] Failed to open script file: " << path << std::endl;
		return;
	}

	std::string scriptCode;
	scriptCode.assign((std::istreambuf_iterator<char>(scriptFile)),
		(std::istreambuf_iterator<char>()));
	scriptFile.close();

	if (scriptCode.empty()) {
		std::cout << "[WARN][ASSETS] Script file is empty: " << path << std::endl;
	}

	m_scriptMap[scriptName] = Script(path, scriptCode);
	std::cout << "[INFO][ASSETS] Added Script: [" << scriptName << "] = " << path << std::endl;
}

void Assets::loadFromFile(const std::string& config) {
	INIReader reader(config);

	auto sections = reader.Sections();

	for (auto section : sections) {
		if (section.starts_with("texture.")) {
			auto name = section.substr(8, section.size());
			auto path = reader.GetString(section, "file", "");
			auto smooth = reader.GetBoolean(section, "smooth", false);
			addTexture(name, path, smooth);
		}
	}

	for (auto section : sections) {
		if (section.starts_with("animation.")) {
			auto name = section.substr(10, section.size());
			auto texture = reader.GetString(section, "texture", "");
			auto frames = reader.GetInteger(section, "frames", 1);
			auto speed = reader.GetInteger(section, "speed", 0);
			addAnimation(name, texture, frames, speed);
		}
	}

	for (auto section : sections) {
		if (section.starts_with("font.")) {
			auto name = section.substr(5, section.size());
			auto path = reader.GetString(section, "file", "");
			addFont(name, path);
		}
	}

	for (auto section : sections) {
		if (section.starts_with("script.")) {
			auto name = section.substr(7, section.size());
			auto path = reader.GetString(section, "file", "");
			addScript(name, path);
		}
	}
}

// -------------------------------------------------------
// Getters

const sf::Texture& Assets::getTexture(const std::string& name) const {
	auto it = m_textureMap.find(name);
	if (it == m_textureMap.end()) {
		throw std::runtime_error("[ASSETS] Texture NotFound");
	}
	return it->second;
}

const Animation& Assets::getAnimation(const std::string& name) const {
	auto it = m_animationMap.find(name);
	if (it == m_animationMap.end()) {
		throw std::runtime_error("[ASSETS] Animation NotFound");
	}
	return it->second;
}

const sf::Font& Assets::getFont(const std::string& name) const {
	auto it = m_fontMap.find(name);
	if (it == m_fontMap.end()) {
		throw std::runtime_error("[ASSETS] Font NotFound");
	}
	return it->second;
}

const Script& Assets::getScript(const std::string& name) const {
	auto it = m_scriptMap.find(name);
	if (it == m_scriptMap.end()) {
		throw std::runtime_error("[ASSETS] Script NotFound");
	}
	return it->second;
}

const std::map<std::string, sf::Texture>& Assets::getTextures() const {
	return m_textureMap;
}

std::map<std::string, Animation>& Assets::getAnimations() {
	return m_animationMap;
}

std::map<std::string, sf::Font>& Assets::getFonts() {
	return m_fontMap;
}

std::map<std::string, Script>& Assets::getScripts() {
	return m_scriptMap;
}
