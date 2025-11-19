#include "GameEngine.hpp"

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Scene.hpp"
#include "Vec2.hpp"

GameEngine::GameEngine() { init(); }

void GameEngine::init() {
	// Register Systems
	m_systems.push_back(std::bind(&GameEngine::sUserInput, this));
}

void GameEngine::update() {
	if (!m_running) { return; }

	if (m_sceneMap.empty()) { return; }

	for (auto system : m_systems) { system(); }

	m_renderTarget.clear();
	currentScene()->update();
	m_renderTarget.display();
}

void GameEngine::quit() { m_running = false; }

sf::RenderTexture& GameEngine::renderTarget() {
	return m_renderTarget;
}

sf::RenderWindow& GameEngine::window() {
	return m_window;
}

std::shared_ptr<Scene> GameEngine::currentScene() {
	return hasScene(m_currentScene) ? m_sceneMap.at(m_currentScene) : nullptr;
}

bool GameEngine::hasScenes() {
	return m_sceneMap.empty();
}

bool GameEngine::hasScene(const std::string& name) {
	return m_sceneMap.find(name) != m_sceneMap.end();
}

void GameEngine::sUserInput() {
	while (auto event = m_window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) { quit(); }

		// this event is triggered when a key is pressed
		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			auto it = currentScene()->getActionMap().find(keyPressed->scancode);
			if (it == currentScene()->getActionMap().end()) { continue; }

			Action action(it->second, "START");
			currentScene()->sDoAction(action);
		}

		// this event is triggered when a key is released
		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
			auto it = currentScene()->getActionMap().find(keyReleased->scancode);
			if (it == currentScene()->getActionMap().end()) {
				continue;
			}

			Action action(it->second, "END");
			currentScene()->sDoAction(action);
		}

		// this event is triggered when we click on window with mouse
		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
			Vec2f mPos(mousePressed->position);
			currentScene()->sClickHandler(mPos, mousePressed->button);
		}


		if (const auto* mousePressed = event->getIf<sf::Event::MouseMoved>()) {
			Vec2f mPos(mousePressed->position);
			currentScene()->sHoverHandler(mPos);
		}
	}
}
