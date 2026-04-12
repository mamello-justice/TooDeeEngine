#include "GameEngine.hpp"

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Physics.hpp"
#include "Scene.hpp"
#include "Vec2.hpp"

GameEngine::GameEngine() {
	init();
}

GameEngine::GameEngine(bool rendering) : m_shouldRender(rendering) {
	init();
}

void GameEngine::init() {
	m_preSceneSystems.push_back(std::bind(&GameEngine::sUserInput, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sMovement, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sScripting, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sCollision, this));
}

void GameEngine::update() {
	if (!m_running) { return; }

	if (m_sceneMap.empty()) { return; }

	for (auto system : m_preSceneSystems) { system(); }

	if (currentScene()) {
		currentScene()->getEntityManager().update();
		currentScene()->update();
	}

	for (auto system : m_postSceneSystems) { system(); }
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

bool GameEngine::getIsRunning() const {
	return m_running;
}

bool GameEngine::hasScenes() const {
	return m_sceneMap.empty();
}

bool GameEngine::hasScene(const std::string& name) const {
	return m_sceneMap.find(name) != m_sceneMap.end();
}

void GameEngine::handleEvent(std::optional<sf::Event> event) {
	if (event->is<sf::Event::Closed>()) { quit(); }

	// this event is triggered when a key is pressed
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
		auto it = currentScene()->getActionMap().find(keyPressed->scancode);
		if (it == currentScene()->getActionMap().end()) { return; }

		Action action(it->second, "START");
		currentScene()->sDoAction(action);
	}

	// this event is triggered when a key is released
	if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
		auto it = currentScene()->getActionMap().find(keyReleased->scancode);
		if (it == currentScene()->getActionMap().end()) { return; }

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

void GameEngine::sUserInput() {
	while (auto event = m_window.pollEvent()) {
		handleEvent(event);
	}
}

void GameEngine::sMovement() {
	if (currentScene()) {
		if (currentScene()->isPaused()) { return; }

		for (const auto& e : currentScene()->getEntityManager().getEntities()) {
			if (e->has<CTransform>()) {
				auto& c = e->get<CTransform>();
				c.pos += c.velocity;
			}
		}
	}
}

void GameEngine::sScripting() {
	if (currentScene()) {
		if (currentScene()->isPaused()) { return; }

		for (const auto& e : currentScene()->getEntityManager().getEntities()) {
			if (e->has<CNativeScript>()) {
				auto& c = e->get<CNativeScript>();
				c.onUpdate(*e);
			}
		}
	}
}

void GameEngine::sCollision() {
	if (currentScene()) {
		if (currentScene()->isPaused()) { return; }

		for (const auto& e : currentScene()->getEntityManager().getEntities()) {
			if (!e->has<CBoundingBox>() && !e->has<CBoundingCircle>()) { continue; }

			for (const auto& e2 : currentScene()->getEntityManager().getEntities()) {
				if (e == e2) { continue; }
				if (!e2->has<CBoundingBox>() && !e2->has<CBoundingCircle>()) { continue; }

				Vec2f overlap = Physics::GetOverlap(e, e2);
				if (overlap.x > 0 && overlap.y > 0) {
					if (e->has<CTransform>()) {
						auto& cTrans = e->get<CTransform>();
					}

					if (e2->has<CTransform>()) {
						auto& cTrans = e->get<CTransform>();
					}
				}

			}

		}
	}

}
