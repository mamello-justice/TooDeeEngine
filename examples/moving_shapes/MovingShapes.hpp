#pragma once

#include <fstream>
#include <functional>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

#include "GameEngine.hpp"
#include "Scene.hpp"

std::istream& operator>>(std::istream&, Vec2f&);

std::istream& operator>>(std::istream&, sf::Color&);

struct CircleConfig {
    std::string name;
    Vec2f pos;
    Vec2f velocity;
    sf::Color color;
    float radius;
};

std::istream& operator>>(std::istream&, CircleConfig&);

struct RectangleConfig {
    std::string name;
    Vec2f pos;
    Vec2f velocity;
    sf::Color color;
    Vec2f size;
};

std::istream& operator>>(std::istream&, RectangleConfig&);

class MovingShapes : public Scene {
protected:
    void init();

    void update() override;
    void sRender() override;

public:
    MovingShapes(std::shared_ptr<GameEngine> gameEngine);

    void loadLevel(const std::string& filename);
    void spawnCircle(const CircleConfig&);
    void spawnRectangle(const RectangleConfig&);
};
