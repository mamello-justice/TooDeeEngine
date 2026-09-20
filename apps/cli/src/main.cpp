#include <string>

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include "quickjs.h"
#endif

#include "spdlog/spdlog.h"

#include "TooDeeCore.hpp"
#include "TooDeeEngine.hpp"

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::info);
    std::filesystem::path base;
    if (argc == 1) {
        base = std::filesystem::current_path();
    }
    else if (argc == 2) {
        base = std::filesystem::path(argv[1]);
    }
    else {
        spdlog::info("usage:\n\t {} <project-path>", argv[0]);
        std::exit(1);
    }

    if (!std::filesystem::is_directory(base)) {
        spdlog::error("\"{}\" is not a directory", base.string());
        std::exit(1);
    }

    std::filesystem::path config = base / "config.tde";
    std::filesystem::path game = base / "main.tde";

    if (!std::filesystem::exists(game)) {
        spdlog::error("main.tde does not exist in {}", base.string());
        std::exit(1);
    }

    std::srand((unsigned int)time(NULL));

    // Load config.tde from . or provided project path
    // Format:
    // What should go here?

    // Load main.tde from . or provided project path
    // Format:
    // - Serialized scene or define list of scenes paths & default scene name

    auto gameEngine = std::make_shared<GameEngine>();

    // Initialize Window
    gameEngine->window().create(sf::VideoMode::getDesktopMode(), "TooDeeEngine");
    gameEngine->window().setFramerateLimit(60);

    // Enable rendering
    gameEngine->m_shouldRender = true;
    if (gameEngine->renderTarget().resize(gameEngine->window().getSize())) {}
    auto scene = std::make_shared<Scene>(gameEngine);

    gameEngine->changeScene("SceneName", scene);

    while (gameEngine->window().isOpen()) {
        while (auto event = gameEngine->window().pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                std::exit(0);
            }
        }

        gameEngine->update();
        Renderer::render(gameEngine);

        gameEngine->window().clear();
        sf::Sprite sprite(gameEngine->renderTarget().getTexture());
        gameEngine->window().draw(sprite);
        gameEngine->window().display();
    }
}
