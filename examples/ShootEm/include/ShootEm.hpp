#pragma once

#include "GameEngine.hpp"

class ShootEm {
    std::shared_ptr<GameEngine> m_gameEngine;

    void init();
public:
    ShootEm();

    void run();
};
