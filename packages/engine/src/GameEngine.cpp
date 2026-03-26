#include "GameEngine.hpp"

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

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

void GameEngine::sRender() {
	m_renderTarget.clear();

	if (m_shouldRender && currentScene())
	{
		for (const auto& e : currentScene()->getEntityManager().getEntities())
		{
			if (e->has<CAnimation>() && e->has<CTransform>())
			{
				auto& cTrans = e->get<CTransform>();
				auto& anim = e->get<CAnimation>().animation;
				anim.getSprite()->setOrigin(anim.getSprite()->getGlobalBounds().size / 2.f);
				anim.getSprite()->setRotation(sf::radians(cTrans.angle));
				anim.getSprite()->setPosition({ cTrans.pos.x, cTrans.pos.y });
				anim.getSprite()->setScale({ cTrans.scale.x, cTrans.scale.y });
				renderTarget().draw(*anim.getSprite());
			}
		}

		// TODO: Remove custom scene rendering after everything is in entity
		currentScene()->sRender();
	}

	m_renderTarget.display();
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
