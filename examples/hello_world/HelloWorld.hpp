#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "TooDeeEngine.hpp"

class HelloWorld : public Scene {
protected:
    void update() override;
    void sRender() override;

public:
    HelloWorld(std::shared_ptr<GameEngine> gameEngine);
};
