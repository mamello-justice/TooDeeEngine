#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjs.h>
#endif

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

	std::vector<std::function<void()>> m_preSceneSystems;
	std::vector<std::function<void()>> m_postSceneSystems;

public:
	bool m_shouldRender = false;

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	JSRuntime* m_jsRuntime = nullptr;
	JSContext* m_jsContext = nullptr;
#endif

	GameEngine();
	GameEngine(bool rendering);
	~GameEngine();

	void init();
	void update();
	void quit();

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	void setupQJSDebug();
#endif

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

	void removeScene(std::shared_ptr<Scene> scene);

	void removeScene(const std::string& name);

	void handleEvent(std::optional<sf::Event> event);

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	void handleJavascriptScriptExecution(std::shared_ptr<Entity> entity);
#endif

	sf::RenderTexture& renderTarget();

	sf::RenderWindow& window();

	void sUserInput();
	void sMovement();
	void sScripting();
	void sCollision();

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	JSValue operator()(JSContext*) const;
#endif
};
