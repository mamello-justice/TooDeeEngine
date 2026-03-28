#include "HelloWorld.hpp"

#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Assets.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"

HelloWorld::HelloWorld(std::shared_ptr<GameEngine> gameEngine)
    : Scene(gameEngine) {}

void HelloWorld::update() {}

void HelloWorld::sRender() {
    sf::Text text(Assets::Instance().getFont("tech"), "Hello World!", 100);
    text.setPosition({
        (m_gameEngine->renderTarget().getSize().x - text.getLocalBounds().size.x) / 2.f,
        (m_gameEngine->renderTarget().getSize().y - text.getLocalBounds().size.y) / 2.f });

    m_gameEngine->renderTarget().draw(text);
}
