#include "ShootEm.hpp"

#include <memory>
#include <string>

#include "GameConfig.hpp"
#include "ShootEm_Menu.hpp"

ShootEm::ShootEm() : m_gameEngine(std::make_shared<GameEngine>()) { init(); }

void ShootEm::init() {
    std::srand((unsigned int)time(NULL));

    GameConfig::getInstance().loadFromFile(getConfigPath());

    m_gameEngine->window().setTitle("ShootEm");
}

void ShootEm::run() {
    m_gameEngine->changeScene(
        "ShootEm_Menu", std::make_shared<ShootEm_Menu>(m_gameEngine));
    m_gameEngine->run();
}
