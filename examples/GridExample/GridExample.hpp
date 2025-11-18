#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "GameEngine.hpp"
#include "Scene.hpp"

class GridExample : public Scene {
protected:
    std::vector<std::function<void()>> m_systems;
    const Vec2f m_gridSize = { 64, 64 };
    sf::Text m_gridText;

    void init();
    void update() override;

    void sRender() override;

public:
    GridExample(std::shared_ptr<GameEngine> gameEngine);
};
