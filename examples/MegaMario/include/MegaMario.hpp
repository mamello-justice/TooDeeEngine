#pragma once

#include "GameEngine.hpp"

class MegaMario {
    bool                        m_running = true;
    std::shared_ptr<GameEngine> m_gameEngine;

    void init();
public:
    MegaMario();

    void run();
    void update();
    void quit();
};
