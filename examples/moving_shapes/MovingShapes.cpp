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

std::istream& operator>>(std::istream& is, Vec2f& v) {
    is >> v.x >> v.y;
    return is;
}

std::istream& operator>>(std::istream& is, sf::Color& color) {
    unsigned int r, g, b;
    is >> r >> g >> b;
    color = sf::Color(r, g, b);
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

void MovingShapes::update() {
    auto wSize = m_gameEngine->renderTarget().getSize();

    for (const auto& e : m_entityManager.getEntities()) {
        if (!e->has<CTransform>()) { continue; }

        auto& cTrans = e->get<CTransform>();

        if (e->has<CBoundingBox>()) {
            auto cCollider = e->get<CBoundingBox>();

            // Vertical
            if ((cTrans.pos.y - cCollider.halfSize.y < 0 && cTrans.velocity.y < 0) ||
                (cTrans.pos.y + cCollider.halfSize.y > wSize.y && cTrans.velocity.y > 0)) {
                cTrans.velocity.y *= -1;
            }
            // Horizontal
            if ((cTrans.pos.x - cCollider.halfSize.x < 0 && cTrans.velocity.x < 0) ||
                (cTrans.pos.x + cCollider.halfSize.x > wSize.x && cTrans.velocity.x > 0)) {
                cTrans.velocity.x *= -1;
            }

        }

        if (e->has<CBoundingCircle>()) {
            auto cCollider = e->get<CBoundingCircle>();

            // Vertical
            if ((cTrans.pos.y - cCollider.radius < 0 && cTrans.velocity.y < 0) ||
                (cTrans.pos.y + cCollider.radius > wSize.y && cTrans.velocity.y > 0)) {
                cTrans.velocity.y *= -1;
            }
            // Horizontal
            if ((cTrans.pos.x - cCollider.radius < 0 && cTrans.velocity.x < 0) ||
                (cTrans.pos.x + cCollider.radius > wSize.x && cTrans.velocity.x > 0)) {
                cTrans.velocity.x *= -1;
            }
        }

    }
}

void MovingShapes::sRender() {}

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
