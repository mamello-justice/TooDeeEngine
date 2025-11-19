#include "ShootEm_Menu.hpp"

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Action.hpp"
#include "Assets.hpp"
#include "Components/Button.hpp"
#include "Core.hpp"
#include "GameEngine.hpp"
#include "Layout/Flex.hpp"
#include "Scene.hpp"

class ShootEm_Play;

ShootEm_Menu::ShootEm_Menu(std::shared_ptr<GameEngine> gameEngine) : Scene(gameEngine) {
	init();
}

void ShootEm_Menu::init() {
	registerAction(sf::Keyboard::Scancode::W, "UP");
	registerAction(sf::Keyboard::Scancode::S, "DOWN");
	registerAction(sf::Keyboard::Scancode::D, "PLAY");
	registerAction(sf::Keyboard::Scancode::Escape, "QUIT");

	m_title = std::make_shared<sf::Text>(
		Assets::Instance().getFont("orbitron"), "Shoot Em!", 150);
	m_title->setFillColor(sf::Color::White);

	m_menuBox = std::make_shared<Layout::Flex>();
	m_menuBox->setGap(50);
	m_menuBox->setPadding(80, 200);

	auto startButton = std::make_shared<Components::Button>(
		std::make_shared<sf::Text>(
			Assets::Instance().getFont("orbitron"),
			"Start",
			50)
	);
	auto onStartHovered = [=](bool hovered) {
		if (hovered) { startButton->setBackgroundColor(sf::Color::Black); }
		else { startButton->setBackgroundColor(sf::Color::Transparent); }
		};
	m_hoverListener.registerListener(
		startButton,
		std::make_shared<std::function<void(bool)>>(onStartHovered)
	);
	auto onStartClicked = [=]() { sDoAction(Action("PLAY", "START")); };
	m_clickListener.registerListener(
		startButton,
		sf::Mouse::Button::Left,
		std::make_shared<std::function<void()>>(onStartClicked));
	m_menuBox->addChild(startButton);

	auto quitButton = std::make_shared<Components::Button>(std::make_shared<sf::Text>(
		Assets::Instance().getFont("orbitron"),
		"Quit",
		50)
	);
	auto onQuitHovered = [=](bool hovered) {
		if (hovered) { quitButton->setBackgroundColor(sf::Color::Black); }
		else { quitButton->setBackgroundColor(sf::Color::Transparent); }
		};
	m_hoverListener.registerListener(
		quitButton,
		std::make_shared<std::function<void(bool)>>(onQuitHovered)
	);
	auto onQuitClicked = [=]() { sDoAction(Action("QUIT", "START")); };
	m_clickListener.registerListener(
		quitButton,
		sf::Mouse::Button::Left,
		std::make_shared<std::function<void()>>(onQuitClicked));
	m_menuBox->addChild(quitButton);

	m_menuBox->setBackgroundColor(sf::Color(0, 0, 0, 100));
}

void ShootEm_Menu::update() {
	sRender();
}

void ShootEm_Menu::onEnd() {
	m_gameEngine->quit();
}

void ShootEm_Menu::sClickHandler(const Vec2f& mPos, const sf::Mouse::Button& button) {
	m_clickListener.onClick(mPos, button);
}

void ShootEm_Menu::sHoverHandler(const Vec2f& mPos) {
	m_hoverListener.onHover(mPos);
}

void ShootEm_Menu::sDoAction(const Action& action) {
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
#ifdef SHOOT_EM_IML
			m_gameEngine->changeScene(
				"PLAY", std::make_shared<ShootEm_Play>(m_gameEngine));
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

void ShootEm_Menu::sRender() {
	m_gameEngine->renderTarget().clear(sf::Color(11, 16, 38));

	auto titleX = (m_gameEngine->renderTarget().getSize().x - m_title->getGlobalBounds().size.x) / 2.f;
	m_title->setPosition(Vec2f(titleX, 100.f));

	auto x = (m_gameEngine->renderTarget().getSize().x - m_menuBox->getSize().x) / 2.f;
	auto y = (m_gameEngine->renderTarget().getSize().y
		+ m_title->getGlobalBounds().size.y
		+ m_title->getGlobalBounds().position.y
		- m_menuBox->getSize().y) / 2.f;
	m_menuBox->setPosition_({ x, y });

	m_gameEngine->renderTarget().draw(*m_menuBox);
	m_gameEngine->renderTarget().draw(*m_title);
}
