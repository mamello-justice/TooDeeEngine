#include "Assets.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>

#include "spdlog/spdlog.h"
#include "INIReader.h"

#include "Animation.hpp"
#include "Script.hpp"

Assets& Assets::Instance() {
	static Assets assets;
	return assets;
}

void Assets::addTexture(const std::string& textureName, const std::filesystem::path& path, bool smooth) {
	m_textureMap[textureName] = sf::Texture();

	if (!m_textureMap[textureName].loadFromFile(path)) {
		m_textureMap.erase(textureName);
		spdlog::error("Could not load texture file: [{}] = {}", textureName, path.string());
	}
	else {
		m_textureMap[textureName].setSmooth(smooth);
		spdlog::info("Loaded Texture: [{}] = {}", textureName, path.string());
	}
}

void Assets::addAnimation(const std::string& animationName, const std::string& textureName, const size_t& frames, const size_t& speed) {
	m_animationMap[animationName] = Animation(animationName, textureName, frames, speed);
}

void Assets::addFont(const std::string& fontName, const std::filesystem::path& path) {
	m_fontMap[fontName] = sf::Font();

	if (!m_fontMap[fontName].openFromFile(path)) {
		m_fontMap.erase(fontName);
		spdlog::error("Could not load font file: [{}] = {}", fontName, path.string());
	}
	else {
		spdlog::info("Loaded Font: [{}] = {}", fontName, path.string());
	}
}

void Assets::addScript(const std::string& scriptName, const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		spdlog::error("Script file not found: [{}] = {}", scriptName, path.string());
		return;
	}

	std::ifstream scriptFile(path);
	if (!scriptFile.is_open()) {
		spdlog::error("Failed to open script file: [{}] = {}", scriptName, path.string());
		return;
	}

	std::string scriptCode;
	scriptCode.assign((std::istreambuf_iterator<char>(scriptFile)),
		(std::istreambuf_iterator<char>()));
	scriptFile.close();

	if (scriptCode.empty()) {
		spdlog::warn("Script file is empty: [{}] = {}", scriptName, path.string());
	}

	m_scriptMap[scriptName] = Script(path.string(), scriptCode);
	spdlog::info("Loaded Script: [{}] = {}", scriptName, path.string());
}

void Assets::reloadScripts() {
	for (const auto& [name, script] : m_scriptMap) {
		addScript(name, script.getPath());
	}
}

void Assets::loadFromFile(const std::filesystem::path& config, const std::filesystem::path& base) {
	auto configPath = base / config;
	INIReader reader(configPath.string());

	auto sections = reader.Sections();

	for (auto section : sections) {
		if (section.starts_with("texture.")) {
			auto name = section.substr(8, section.size());
			auto path = reader.GetString(section, "file", "");
			auto smooth = reader.GetBoolean(section, "smooth", false);
			addTexture(name, base / std::filesystem::canonical(path), smooth);
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
			addFont(name, base / std::filesystem::canonical(path));
		}
	}

	for (auto section : sections) {
		if (section.starts_with("script.")) {
			auto name = section.substr(7, section.size());
			auto path = reader.GetString(section, "file", "");
			addScript(name, base / std::filesystem::canonical(path));
		}
	}
}

const sf::Texture& Assets::getTexture(const std::string& name) const {
	auto it = m_textureMap.find(name);
	if (it == m_textureMap.end())
		throw std::runtime_error("Texture NotFound");
	return it->second;
}

const Animation& Assets::getAnimation(const std::string& name) const {
	auto it = m_animationMap.find(name);
	if (it == m_animationMap.end())
		throw std::runtime_error("Animation NotFound");
	return it->second;
}

const sf::Font& Assets::getFont(const std::string& name) const {
	auto it = m_fontMap.find(name);
	if (it == m_fontMap.end())
		throw std::runtime_error("Font NotFound");
	return it->second;
}

const Script& Assets::getScript(const std::string& name) const {
	auto it = m_scriptMap.find(name);
	if (it == m_scriptMap.end())
		throw std::runtime_error("Script NotFound");
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
