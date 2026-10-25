#include <filesystem>
#include <ctime>
#include <string>
#include <exception>

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include "quickjs.h"
#endif

#include "spdlog/spdlog.h"

#include "TooDeeCore.hpp"
#include "TooDeeEngine.hpp"
#include "Project.hpp"

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::info);
    if (argc > 2) {
        spdlog::error("usage: {} [project-path|main.ini|main.tde]", argv[0]);
        return 1;
    }

    try {
        const auto entry = argc == 2 ? std::filesystem::path(argv[1]) : std::filesystem::current_path();
        const auto project = loadProject(resolveProjectEntry(entry));
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        auto gameEngine = std::make_shared<GameEngine>();
        initializeProject(project, gameEngine);

        while (gameEngine->window().isOpen() && gameEngine->getIsRunning()) {
            gameEngine->update();
            if (!gameEngine->getIsRunning()) break;

            Renderer::render(gameEngine);
            gameEngine->window().clear();
            gameEngine->window().draw(sf::Sprite(gameEngine->renderTarget().getTexture()));
            gameEngine->window().display();
        }
    }
    catch (const std::exception& error) {
        spdlog::error("{}", error.what());
        return 1;
    }
}
