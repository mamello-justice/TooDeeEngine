#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

#include "TooDeeEngine.hpp"
#include "NativeScripting.hpp"

int main(int argc, char* argv[]) {
    std::string configPath;
    if (argc == 1) {
        configPath = "config.ini";
    }
    else if (argc == 2) {
        configPath = argv[1];
    }
    else {
        std::cout << "usage:\n\t" << argv[0] << " <config-path>" << std::endl;
        std::exit(1);
    }

    std::srand((unsigned int)time(NULL));

    Assets::Instance().loadFromFile(configPath);

    // Create Game Engine
    auto gameEngine = std::make_shared<GameEngine>();

    // Init Window
    gameEngine->window().create(sf::VideoMode({ 900, 600 }), "Native/C++ Scripting");
    gameEngine->window().setFramerateLimit(60);

    // Enable rendering
    gameEngine->m_shouldRender = true;
    if (gameEngine->renderTarget().resize(gameEngine->window().getSize())) {}

    // Create scene
    auto scene = std::make_shared<NativeScripting::Example>(gameEngine);
    scene->loadLevel("levels/level.txt");
    gameEngine->changeScene("NativeScripting", scene);
    scene->setPaused(false);

    while (true) {
        while (auto event = gameEngine->window().pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                std::exit(0);
            }
        }

        gameEngine->update();

        gameEngine->window().clear();
        Renderer::render(gameEngine);
        gameEngine->window().draw(sf::Sprite(gameEngine->renderTarget().getTexture()));
        gameEngine->window().display();
    }
}
