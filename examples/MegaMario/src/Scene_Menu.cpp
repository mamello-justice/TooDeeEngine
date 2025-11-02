#include "Scene_Menu.hpp"

#include <SFML/Graphics.hpp>

#include "Action.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"
#include "Scene_Play.hpp"

Scene_Menu::Scene_Menu(std::shared_ptr<GameEngine> gameEngine) : Scene(gameEngine) {
	init();
}

void Scene_Menu::init() {
	registerAction(sf::Keyboard::Scancode::W, "UP");
	registerAction(sf::Keyboard::Scancode::S, "DOWN");
	registerAction(sf::Keyboard::Scancode::D, "PLAY");
	registerAction(sf::Keyboard::Scancode::Escape, "QUIT");

	registerLevel("Level 1", "level1.txt");
	registerLevel("Level 2", "level2.txt");
	registerLevel("Level 3", "level3.txt");
}

void Scene_Menu::update() {
	sRender();
}

void Scene_Menu::onEnd() {
	m_gameEngine->quit();
}

void Scene_Menu::sDoAction(const Action& action) {
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			if (m_selectedMenuIndex > 0)
			{
				m_selectedMenuIndex--;
			}
			else
			{
				m_selectedMenuIndex = m_menuStrings.size() - 1;
			}
		}
		else if (action.name() == "DOWN")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			m_gameEngine->changeScene("PLAY", std::make_shared<Scene_Play>(
				m_gameEngine,
				m_levelPaths[m_selectedMenuIndex]));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
	else if (action.type() == "END")
	{
	}
}

void Scene_Menu::sRender() {
	m_gameEngine->window().clear(sf::Color(97, 133, 248));

	sf::Text title(Assets::Instance().getFont("Mario"), "MEGA MARIO", 100);
	title.setFillColor(sf::Color::White);

	sf::RectangleShape titleBox(
		Vec2f(title.getGlobalBounds().size.x + 100.f, title.getGlobalBounds().size.y + 100.f));
	titleBox.setFillColor(sf::Color(184, 64, 4));
	titleBox.setPosition(
		Vec2f(
			(m_gameEngine->window().getSize().x - titleBox.getSize().x) / 2.f - 20.f,
			40.f));
	title.setPosition(
		Vec2f(
			(m_gameEngine->window().getSize().x - title.getLocalBounds().size.x) / 2.f - 20.f,
			(titleBox.getGlobalBounds().size.y - title.getLocalBounds().size.y) / 2.f + 20.f));

	m_gameEngine->window().draw(titleBox);
	m_gameEngine->window().draw(title);

	auto offsetY = (m_gameEngine->window().getSize().y - (75.f * m_menuStrings.size())) / 2.f;

	for (size_t i = 0; i < m_menuStrings.size(); i++) {
		auto level = m_menuStrings[i];

		sf::Text text(Assets::Instance().getFont("Mario"), level, 75);
		text.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color::Black);
		text.setPosition(
			Vec2f(
				(m_gameEngine->window().getSize().x - text.getLocalBounds().size.x) / 2.f - 20.f,
				offsetY + (text.getLocalBounds().size.y + 20.f) * i));
		m_gameEngine->window().draw(text);
	}
}

void Scene_Menu::registerLevel(const std::string& name, const std::string& path) {
	m_menuStrings.push_back(name);
	m_levelPaths.push_back(path);
}
