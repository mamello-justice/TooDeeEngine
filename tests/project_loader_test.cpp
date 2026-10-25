#include <chrono>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "Project.hpp"
#include "Components.hpp"
#include "GameEngine.hpp"
#include "RuntimeSerializer.hpp"

namespace
{
    class CustomScene final : public Scene {
    public:
        using Scene::Scene;
    };

    class TemporaryProject {
        std::filesystem::path m_path;

    public:
        TemporaryProject() {
            const auto token = std::chrono::steady_clock::now().time_since_epoch().count();
            m_path = std::filesystem::temp_directory_path() / ("toodee-project-test-" + std::to_string(token));
            std::filesystem::create_directories(m_path);
        }

        ~TemporaryProject() {
            std::error_code error;
            std::filesystem::remove_all(m_path, error);
        }

        const std::filesystem::path& path() const { return m_path; }

        void write(const std::filesystem::path& relative, const std::string& content) const {
            const auto file = m_path / relative;
            std::filesystem::create_directories(file.parent_path());
            std::ofstream output(file);
            if (!output) throw std::runtime_error("Could not create test fixture file");
            output << content;
        }
    };
}

TEST(ProjectLoader, LoadsInlineAndReferencedSectionsRelativeToTheirOwner) {
    TemporaryProject project;
    project.write("main.ini",
        "[project]\n"
        "version=1\n"
        "default_scene=menu\n"
        "[config.assets]\n"
        "file=assets.ini\n"
        "[window]\n"
        "file=settings/window.ini\n"
        "[scene.startup]\n"
        "name=Startup\n"
        "[scene.menu]\n"
        "file=scenes/menu.ini\n");
    project.write("assets.ini", "[protocol]\nversion=1.0\n");
    project.write("settings/window.ini",
        "[window]\n"
        "title=Loader Test\n"
        "width=900\n"
        "height=600\n"
        "frame_rate=120\n"
        "mode=windowed\n"
        "style=resize\n");
    project.write("scenes/menu.ini", "[scene.menu]\nname=Menu\n");

    const auto definition = loadProject(project.path());
    EXPECT_EQ(definition.sourcePath.filename(), "main.ini");
    ASSERT_EQ(definition.assetConfigPaths.size(), 2U);
    EXPECT_EQ(definition.assetConfigPaths[1].filename(), "assets.ini");
    EXPECT_EQ(definition.defaultScene, "menu");
    EXPECT_EQ(definition.windowTitle, "Loader Test");
    EXPECT_EQ(definition.windowWidth, 900U);
    EXPECT_EQ(definition.windowHeight, 600U);
    EXPECT_EQ(definition.frameRateLimit, 120U);
    EXPECT_EQ(definition.scenes.size(), 2U);
    const auto startup = std::find_if(definition.scenes.begin(), definition.scenes.end(), [](const ProjectScene& scene) { return scene.id == "startup"; });
    const auto menu = std::find_if(definition.scenes.begin(), definition.scenes.end(), [](const ProjectScene& scene) { return scene.id == "menu"; });
    ASSERT_NE(startup, definition.scenes.end());
    ASSERT_NE(menu, definition.scenes.end());
    EXPECT_EQ(startup->name, "Startup");
    EXPECT_EQ(menu->name, "Menu");
    EXPECT_EQ(menu->sourcePath, project.path() / "scenes/menu.ini");
}

TEST(ProjectLoader, AcceptsTdeManifestAndDefaultsSingleScene) {
    TemporaryProject project;
    project.write("main.tde",
        "[project]\n"
        "version=1\n"
        "[scene.startup]\n"
        "name=Startup\n");

    const auto definition = loadProject(project.path() / "main.tde");
    EXPECT_EQ(definition.defaultScene, "startup");
    ASSERT_EQ(definition.assetConfigPaths.size(), 1U);
    EXPECT_EQ(definition.assetConfigPaths.front(), project.path() / "main.tde");
}

TEST(ProjectLoader, LoadsMultipleNamedConfigFiles) {
    TemporaryProject project;
    project.write("main.tde",
        "[project]\n"
        "version=1\n"
        "[config.fonts]\n"
        "file=config/fonts.ini\n"
        "[config.textures]\n"
        "file=config/textures.ini\n"
        "[config.whatever]\n"
        "file=config/animations.ini\n"
        "[scene.startup]\n"
        "name=Startup\n");
    project.write("config/fonts.ini", "[font.default]\nfile=default.ttf\n");
    project.write("config/textures.ini", "[texture.player]\nfile=player.png\n");
    project.write("config/animations.ini", "[animation.walk]\ntexture=player\nframes=4\n");

    const auto definition = loadProject(project.path());
    ASSERT_EQ(definition.assetConfigPaths.size(), 4U);
    EXPECT_EQ(definition.assetConfigPaths[1], project.path() / "config/fonts.ini");
    EXPECT_EQ(definition.assetConfigPaths[2], project.path() / "config/textures.ini");
    EXPECT_EQ(definition.assetConfigPaths[3], project.path() / "config/animations.ini");
}

TEST(GameEngine, LoadsWindowSettingsFromFile) {
    TemporaryProject files;
    files.write("with-icon.ini",
        "[window]\n"
        "width=900\n"
        "height=600\n"
        "icon=icon.png\n");
    files.write("icon.png", "");

    GameEngine gameEngine(false);
    EXPECT_NO_THROW(gameEngine.loadSettingsFromFile(files.path() / "with-icon.ini"));
    EXPECT_THROW(gameEngine.loadSettingsFromFile(files.path() / "missing.ini"), std::runtime_error);
}

TEST(ProjectLoader, RejectsAmbiguousDirectoryEntrypoints) {
    TemporaryProject project;
    project.write("main.ini", "[project]\nversion=1\n[scene.startup]\n");
    project.write("main.tde", "[project]\nversion=1\n[scene.startup]\n");
    EXPECT_THROW(resolveProjectEntry(project.path()), std::runtime_error);
}

TEST(ProjectLoader, RejectsMissingDefaultScene) {
    TemporaryProject project;
    project.write("main.ini",
        "[project]\n"
        "version=1\n"
        "default_scene=missing\n"
        "[scene.startup]\n");
    EXPECT_THROW(loadProject(project.path()), std::runtime_error);
}

TEST(ProjectLoader, BuildsNestedEntitiesAndComponents) {
    TemporaryProject project;
    project.write("main.tde",
        "[project]\n"
        "version=1\n"
        "default_scene=hello\n"
        "[scene.hello]\n"
        "name=HelloWorld\n"
        "[scene.hello.entity.my_entity]\n"
        "tag=text\n"
        "[scene.hello.entity.my_entity.component.label]\n"
        "text=Hello there\n"
        "font=default\n"
        "size=30\n"
        "[scene.hello.entity.my_entity.component.transform]\n"
        "x=40\n"
        "y=80\n"
        "[scene.hello.action.space]\n"
        "name=jump\n");

    const auto definition = loadProject(project.path());
    auto gameEngine = std::make_shared<GameEngine>(false);
    loadProjectScenes(definition, gameEngine);
    ASSERT_NE(gameEngine->currentScene(), nullptr);
    auto& entities = gameEngine->currentScene()->getEntityManager();
    entities.update();
    ASSERT_EQ(entities.getEntities().size(), 1U);
    const auto& entity = entities.getEntities().front();
    EXPECT_EQ(entity->tag(), "text");
    ASSERT_TRUE(entity->has<CLabel>());
    EXPECT_EQ(entity->get<CLabel>().label, "Hello there");
    ASSERT_TRUE(entity->has<CTransform>());
    EXPECT_FLOAT_EQ(entity->get<CTransform>().pos.x, 40.f);
    EXPECT_FLOAT_EQ(entity->get<CTransform>().pos.y, 80.f);
    EXPECT_EQ(gameEngine->currentScene()->getActionMap().at(sf::Keyboard::Scan::Space), "jump");
}

TEST(RuntimeSerializer, RoundTripsCppCreatedScenesAndEntities) {
    TemporaryProject files;
    auto source = std::make_shared<GameEngine>(false);
    auto menu = std::make_shared<Scene>(source);
    auto menuEntity = menu->getEntityManager().addEntity("ui", 8);
    menuEntity->add<CLabel>("Saved label", "default", 24);
    menuEntity->add<CTransform>(Vec2f(15.f, 25.f));
    auto game = std::make_shared<Scene>(source);
    game->setPaused(false);
    game->registerAction(sf::Keyboard::Scan::Space, "jump");
    auto player = game->getEntityManager().addEntity("player", 42);
    player->add<CTransform>(Vec2f(12.f, 34.f), Vec2f(1.f, -2.f));
    player->get<CTransform>().prevPos = Vec2f(10.f, 30.f);
    player->add<CScore>(7);
    player->add<CGravity>(0.5f);
    source->changeScene("menu", menu);
    source->changeScene("game", game);

    const auto savePath = files.path() / "slot.ini";
    RuntimeSerializer::save(source, savePath);
    auto restored = std::make_shared<GameEngine>(false);
    auto customGameScene = std::make_shared<CustomScene>(restored);
    restored->changeScene("game", customGameScene);
    RuntimeSerializer::load(restored, savePath);

    EXPECT_TRUE(restored->hasScene("menu"));
    EXPECT_TRUE(restored->hasScene("game"));
    EXPECT_EQ(restored->currentScene(), restored->getScene("game"));
    EXPECT_EQ(restored->getScene("game"), customGameScene);
    EXPECT_FALSE(restored->getScene("game")->isPaused());
    EXPECT_EQ(restored->getScene("game")->getActionMap().at(sf::Keyboard::Scan::Space), "jump");
    const auto gameEntities = restored->getScene("game")->getEntityManager().getEntities();
    ASSERT_EQ(gameEntities.size(), 1U);
    EXPECT_EQ(gameEntities.front()->id(), 42U);
    EXPECT_EQ(gameEntities.front()->tag(), "player");
    ASSERT_TRUE(gameEntities.front()->has<CTransform>());
    EXPECT_FLOAT_EQ(gameEntities.front()->get<CTransform>().pos.x, 12.f);
    EXPECT_FLOAT_EQ(gameEntities.front()->get<CTransform>().velocity.y, -2.f);
    EXPECT_FLOAT_EQ(gameEntities.front()->get<CTransform>().prevPos.x, 10.f);
    EXPECT_FLOAT_EQ(gameEntities.front()->get<CTransform>().prevPos.y, 30.f);
    EXPECT_EQ(gameEntities.front()->get<CScore>().score, 7);
    EXPECT_FLOAT_EQ(gameEntities.front()->get<CGravity>().gravity, 0.5f);
    const auto menuEntities = restored->getScene("menu")->getEntityManager().getEntities();
    ASSERT_EQ(menuEntities.size(), 1U);
    EXPECT_EQ(menuEntities.front()->get<CLabel>().label, "Saved label");
}

TEST(RuntimeSerializer, RejectsInvalidSaveWithoutReplacingExistingRuntime) {
    TemporaryProject files;
    auto gameEngine = std::make_shared<GameEngine>(false);
    auto scene = std::make_shared<Scene>(gameEngine);
    gameEngine->changeScene("original", scene);
    files.write("invalid-save.ini", "[save]\nversion=99\n");

    EXPECT_THROW(RuntimeSerializer::load(gameEngine, files.path() / "invalid-save.ini"), std::runtime_error);
    EXPECT_EQ(gameEngine->currentScene(), scene);
    EXPECT_TRUE(gameEngine->hasScene("original"));
}
