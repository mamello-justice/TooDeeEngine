#include "MegaMario.hpp"

#include <memory>
#include <string>

#include "MegaMario_Menu.hpp"
#include "MegaMario_Play.hpp"

MegaMario::MegaMario() : m_gameEngine(std::make_shared<GameEngine>()) { init(); }

void MegaMario::init() {
    std::srand((unsigned int)time(NULL));

    m_gameEngine->window().setTitle("MegaMario");
}

void MegaMario::run() {
    m_gameEngine->changeScene(
        "MegaMario_Menu", std::make_shared<MegaMario_Menu>(m_gameEngine));

    while (m_running) { update(); }
}

void MegaMario::update() {
    m_gameEngine->update();
}

void MegaMario::quit() {
    m_running = false;
}
