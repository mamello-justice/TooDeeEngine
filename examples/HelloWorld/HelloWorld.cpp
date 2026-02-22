#include "HelloWorld.hpp"

#include <iostream>
#include <memory>

#include "GameEngine.hpp"
#include "Scene.hpp"

HelloWorld::HelloWorld(std::shared_ptr<GameEngine> gameEngine)
    : Scene(gameEngine) {
    init();
}

void HelloWorld::init() {
    // Register Systems
    m_systems.push_back(std::bind(&HelloWorld::sRender, this));
}

void HelloWorld::update() {
    for (auto system : m_systems) { system(); }
}

// Systems
void HelloWorld::sRender() {
    sf::Text text(Assets::Instance().getFont("tech"), "Hello World!", 100);
    text.setPosition({
        (m_gameEngine->renderTarget().getSize().x - text.getLocalBounds().size.x) / 2.f,
        (m_gameEngine->renderTarget().getSize().y - text.getLocalBounds().size.y) / 2.f });

    m_gameEngine->renderTarget().draw(text);
}
