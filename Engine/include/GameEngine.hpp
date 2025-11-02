#pragma once

#include <map>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

class Scene;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine {
protected:
	SceneMap            m_sceneMap;
	sf::RenderWindow    m_window;
	std::string         m_currentScene;
	size_t              m_simulationSpeed = 1;
	bool                m_running = true;
	sf::Clock           m_deltaClock;

public:
	GameEngine();

	void init();
	void run();
	void update();
	void quit();

	template <class T>
	void changeScene(const std::string& name, std::shared_ptr<T> scene) {
		m_window.setView(m_window.getDefaultView());

		m_currentScene = name;
		if (m_sceneMap.find(name) == m_sceneMap.end())
		{
			m_sceneMap[name] = scene;
		}
	}

	std::shared_ptr<Scene> currentScene();

	sf::RenderWindow& window();

	void sUserInput();
};
