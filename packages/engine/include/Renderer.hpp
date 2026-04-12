#pragma once

#include <memory>

#include "GameEngine.hpp"

class Renderer {
public:
    static void render(std::shared_ptr<GameEngine> engine);
};
