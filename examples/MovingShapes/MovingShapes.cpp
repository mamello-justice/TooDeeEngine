#include "MovingShapes.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <print>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>

#include "Components.hpp"
#include "GameEngine.hpp"
#include "INIReader.h"
#include "Scene.hpp"

static void centerTransform(const sf::Shape& container, sf::Text& target) {
    auto cBounds = container.getGlobalBounds();
    auto tBounds = target.getLocalBounds();

    auto cCenterX = cBounds.size.x / 2;
    auto tCenterX = tBounds.size.x / 2;
    auto tPositionX = cBounds.position.x + cCenterX - tCenterX;

    auto cCenterY = cBounds.size.y / 2;
    auto tCenterY = (float)target.getCharacterSize() / 2;
    auto tPositionY = cBounds.position.y + cCenterY - tCenterY;

    target.setPosition({ tPositionX, tPositionY });
}

std::istream& operator>>(std::istream& is, Vec2f& v) {
    is >> v.x >> v.y;
    return is;
}

std::istream& operator>>(std::istream& is, sf::Color& color) {
    is >> color.r >> color.g >> color.b;
    return is;
}

std::istream& operator>>(std::istream& is, CircleConfig& config) {
    is
        >> config.name
        >> config.pos
        >> config.velocity
        >> config.color
        >> config.radius;
    return is;
}

std::istream& operator>>(std::istream& is, RectangleConfig& config) {
    is
        >> config.name
        >> config.pos
        >> config.velocity
        >> config.color
        >> config.size;
    return is;
}

MovingShapes::MovingShapes(std::shared_ptr<GameEngine> gameEngine) :
    Scene(gameEngine) {
    init();
}

void MovingShapes::init() {}

void MovingShapes::loadLevel(const std::string& filename) {
    std::ifstream file(filename);
    std::string str;

    while (file.good()) {
        file >> str;
        if (str == "Circle") {
            CircleConfig config;
            file >> config;
            spawnCircle(config);
        }
        else if (str == "Rectangle") {
            RectangleConfig config;
            file >> config;
            spawnRectangle(config);
        }
    }
}

void MovingShapes::update() {
    sMovement();
    sCollision();
    sRender();
}

void MovingShapes::sCollision() {
    for (auto& e : m_entityManager.getEntities()) {
        if (e->has<CTransform>() && e->has<CCircle>() && e->has<CBoundingBox>()) {
            auto& cShape = e->get<CCircle>();
            auto& cTrans = e->get<CTransform>();
            auto bounds = cShape.circle.getGlobalBounds();
            auto wSize = m_gameEngine->renderTarget().getSize();
            // Vertical
            if (bounds.position.y <= 0 and cTrans.velocity.y < 0
                or bounds.position.y + bounds.size.y >= wSize.y and cTrans.velocity.y > 0) {
                cTrans.velocity.y *= -1;
            }
            // Horizontal
            if (bounds.position.x <= 0 and cTrans.velocity.x < 0
                or bounds.position.x + bounds.size.x >= wSize.x and cTrans.velocity.x > 0) {
                cTrans.velocity.x *= -1;
            }
        }
    }
}

void MovingShapes::sMovement() {
    for (auto& e : m_entityManager.getEntities()) {
        if (e->has<CTransform>()) {
            auto& c = e->get<CTransform>();
            c.pos += c.velocity;
        }
    }
}

void MovingShapes::sRender() {
    m_gameEngine->renderTarget().clear();

    for (auto& e : m_entityManager.getEntities()) {
        if (e->has<CCircle>()) {
            auto& circle = e->get<CCircle>().circle;
            m_gameEngine->renderTarget().draw(circle);

            if (e->has<CLabel>()) {
                auto& cLabel = e->get<CLabel>();
                auto font = Assets::Instance().getFont("tech");
                sf::Text text(font, cLabel.label, 24);
                centerTransform(circle, text);
                m_gameEngine->renderTarget().draw(text);
            }
        }
        if (e->has<CRectangle>()) {
            auto& rect = e->get<CRectangle>().rect;
            m_gameEngine->renderTarget().draw(rect);

            if (e->has<CLabel>()) {
                auto& cLabel = e->get<CLabel>();
                auto font = Assets::Instance().getFont("tech");
                sf::Text text(font, cLabel.label, 24);
                centerTransform(rect, text);
                m_gameEngine->renderTarget().draw(text);
            }
        }
    }
}

void MovingShapes::spawnCircle(const CircleConfig& config) {
    auto e = m_entityManager.addEntity("shape");

    e->add<CLabel>(config.name);
    e->add<CTransform>(config.pos, config.velocity);
    e->add<CCircle>(config.radius, 32, config.color, config.color, 0.f);
    e->add<CBoundingCircle>(config.radius);
}

void MovingShapes::spawnRectangle(const RectangleConfig& config) {
    auto e = m_entityManager.addEntity("shape");

    e->add<CLabel>(config.name);
    e->add<CTransform>(config.pos, config.velocity);
    e->add<CRectangle>(config.size, config.color, config.color, 0.f);
    e->add<CBoundingBox>(config.size);
}
