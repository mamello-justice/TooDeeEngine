#include "GridExample.hpp"

#include <iostream>
#include <memory>

#include "GameEngine.hpp"
#include "Scene.hpp"

GridExample::GridExample(std::shared_ptr<GameEngine> gameEngine) :
    Scene(gameEngine),
    m_gridText(Assets::Instance().getFont("tech"), "", 10) {
    init();
}

void GridExample::init() {
    // Register Systems
    m_systems.push_back(std::bind(&GridExample::sRender, this));
}

void GridExample::update() {
    for (auto system : m_systems) { system(); }
}

// Systems
void GridExample::sRender() {
    auto wWidth = m_gameEngine->renderTarget().getSize().x;
    auto wHeight = m_gameEngine->renderTarget().getSize().y;

    sf::Text text(Assets::Instance().getFont("tech"), "Hello World!", 100);
    text.setPosition({
        (wWidth - text.getLocalBounds().size.x) / 2.f,
        (wHeight - text.getLocalBounds().size.y) / 2.f });

    m_gameEngine->renderTarget().draw(text);

    float leftX = float(m_gameEngine->renderTarget().getView().getCenter().x) - wWidth / 2.0f;
    float rightX = leftX + wWidth + m_gridSize.x;
    float nextGridX = leftX - float((int)leftX % (int)m_gridSize.x);

    for (float x = nextGridX; x < rightX; x += float(m_gridSize.x)) {
        sf::Vertex line[] = {
                sf::Vertex{Vec2f(x, 0)},
                sf::Vertex{Vec2f(x, wHeight)}
        };

        m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);
    }

    for (float y = 0; y < wHeight; y += float(m_gridSize.y)) {
        sf::Vertex line[] = {
                sf::Vertex{Vec2f(leftX, wHeight - y)},
                sf::Vertex{Vec2f(rightX, wHeight - y)}
        };

        m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);

        for (float x = nextGridX; x < rightX; x += float(m_gridSize.x)) {
            std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
            std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
            m_gridText.setString("(" + xCell + "," + yCell + ")");
            m_gridText.setPosition({ x + 3, wHeight - y - m_gridSize.y + 2 });
            m_gameEngine->renderTarget().draw(m_gridText);
        }
    }
}
