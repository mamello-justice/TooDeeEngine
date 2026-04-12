#include "TypescriptScripting.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <print>
#include <sstream>
#include <string>

#include <SFML/Graphics.hpp>

#include "TooDeeEngine.hpp"
#include "INIReader.h"

namespace TypeScriptScripting
{
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

    Example::Example(std::shared_ptr<GameEngine> gameEngine) :
        Scene(gameEngine) {
        init();
    }

    void Example::init() {}

    void Example::update() {}

    void Example::sRender() {}

    void Example::loadLevel(const std::string& filename) {
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

    void Example::spawnCircle(const CircleConfig& config) {
        auto e = m_entityManager.addEntity("shape");

        e->add<CLabel>(config.name);
        e->add<CTransform>(config.pos, config.velocity);
        e->add<CCircle>(config.radius, 32, config.color, config.color, 0.f);
        e->add<CBoundingCircle>(config.radius);

#ifdef TOO_DEE_ENGINE_JAVASCRIPT_SCRIPTING
        e->add<CJavascriptScript>("cboundingcircle_script");
#endif
    }

    void Example::spawnRectangle(const RectangleConfig& config) {
        auto e = m_entityManager.addEntity("shape");

        e->add<CLabel>(config.name);
        e->add<CTransform>(config.pos, config.velocity);
        e->add<CRectangle>(config.size, config.color, config.color, 0.f);
        e->add<CBoundingBox>(config.size);

#ifdef TOO_DEE_ENGINE_JAVASCRIPT_SCRIPTING
        e->add<CJavascriptScript>("cboundingbox_script");
#endif
    }
} // namespace TypescriptScripting
