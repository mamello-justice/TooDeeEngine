#include "MegaMario_Menu.hpp"

#include <SFML/Graphics.hpp>

#include "Action.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"

class MegaMario_Play;

MegaMario_Menu::MegaMario_Menu(std::shared_ptr<GameEngine> gameEngine)
	: Scene(gameEngine) {
	init();
}

void MegaMario_Menu::init() {
	Assets::Instance().loadFromFile("bin/MegaMario/config.ini");

	// Register Actions
	registerAction(sf::Keyboard::Scancode::W, "UP");
	registerAction(sf::Keyboard::Scancode::S, "DOWN");
	registerAction(sf::Keyboard::Scancode::D, "PLAY");
	registerAction(sf::Keyboard::Scancode::Escape, "QUIT");

	// Register Levels
	registerLevel("Level 1", "level1.txt");
	registerLevel("Level 2", "level2.txt");
	registerLevel("Level 3", "level3.txt");

	// Register Systems
	m_systems.push_back(std::bind(&MegaMario_Menu::sRender, this));
}

void MegaMario_Menu::update() {
	for (auto system : m_systems) { system(); }
}

void MegaMario_Menu::onEnd() {
	m_gameEngine->quit();
}

void MegaMario_Menu::registerLevel(const std::string& name, const std::string& path) {
	m_menuStrings.push_back(name);
	m_levelPaths.push_back(path);
}

// Systesm
void MegaMario_Menu::sDoAction(const Action& action) {
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
#ifdef MEGA_MARIO_IMPL
			m_gameEngine->changeScene("PLAY", std::make_shared<MegaMario_Play>(
				m_gameEngine,
				m_levelPaths[m_selectedMenuIndex]));
#endif
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

void MegaMario_Menu::sRender() {
	m_gameEngine->renderTarget().clear(sf::Color(97, 133, 248));

	sf::Text title(Assets::Instance().getFont("mario"), "MEGA MARIO", 100);
	title.setFillColor(sf::Color::White);

	sf::RectangleShape titleBox(
		Vec2f(title.getGlobalBounds().size.x + 100.f, title.getGlobalBounds().size.y + 100.f));
	titleBox.setFillColor(sf::Color(184, 64, 4));
	titleBox.setPosition(
		Vec2f(
			(m_gameEngine->renderTarget().getSize().x - titleBox.getSize().x) / 2.f - 20.f,
			40.f));
	title.setPosition(
		Vec2f(
			(m_gameEngine->renderTarget().getSize().x - title.getLocalBounds().size.x) / 2.f - 20.f,
			(titleBox.getGlobalBounds().size.y - title.getLocalBounds().size.y) / 2.f + 20.f));

	m_gameEngine->renderTarget().draw(titleBox);
	m_gameEngine->renderTarget().draw(title);

	auto offsetY = (m_gameEngine->renderTarget().getSize().y - (75.f * m_menuStrings.size())) / 2.f;

	for (size_t i = 0; i < m_menuStrings.size(); i++) {
		auto level = m_menuStrings[i];

		sf::Text text(Assets::Instance().getFont("mario"), level, 75);
		text.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color::Black);
		text.setPosition(
			Vec2f(
				(m_gameEngine->renderTarget().getSize().x - text.getLocalBounds().size.x) / 2.f - 20.f,
				offsetY + (text.getLocalBounds().size.y + 20.f) * i));
		m_gameEngine->renderTarget().draw(text);
	}
}
