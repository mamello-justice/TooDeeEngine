#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "GameEngine.hpp"
#include "Scene.hpp"

class HelloWorld : public Scene {
protected:
    std::vector<std::function<void()>> m_systems;

    void init();
    void update() override;

    void sRender() override;

public:
    HelloWorld(std::shared_ptr<GameEngine> gameEngine);
};
