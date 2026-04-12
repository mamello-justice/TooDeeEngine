#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "Scene.hpp"

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine {
protected:
	SceneMap            m_sceneMap;
	sf::RenderWindow    m_window;
	sf::RenderTexture	m_renderTarget;
	std::string         m_currentScene;
	size_t              m_simulationSpeed = 1;
	bool                m_running = true;
	sf::Clock           m_deltaClock;
	std::vector<std::function<void()>> m_preSceneSystems;
	std::vector<std::function<void()>> m_postSceneSystems;

public:
	bool				m_shouldRender = false;

	GameEngine();
	GameEngine(bool rendering);

	void init();
	void update();
	void quit();

	template <class T>
	void changeScene(const std::string& name, std::shared_ptr<T> scene) {
		m_currentScene = name;
		if (m_sceneMap.find(name) == m_sceneMap.end())
		{
			m_sceneMap[name] = scene;
		}
	}

	std::shared_ptr<Scene> currentScene();

	bool getIsRunning() const;

	bool hasScenes() const;

	bool hasScene(const std::string& name) const;

	void handleEvent(std::optional<sf::Event> event);

	sf::RenderTexture& renderTarget();

	sf::RenderWindow& window();

	void sUserInput();
	void sMovement();
	void sScripting();
	void sCollision();
};
