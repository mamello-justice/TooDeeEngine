#include "Editor.hpp"

#include <format>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "Components.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"
#include "Styles.hpp"
#include "Vec2.hpp"

#ifdef BUILD_EXAMPLES
#include "Examples.hpp"
#endif

Editor::Editor() :
    m_gameEngine(std::make_shared<GameEngine>()) {
    init();
}

void Editor::init() {
    std::srand((unsigned int)time(NULL));

    // Initialize Window
    window().create(sf::VideoMode::getDesktopMode(), "TooDeeEditor");
    window().setFramerateLimit(60);

    // Initialize ImGui
    if (!ImGui::SFML::Init(window())) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        exit(1);
    }
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
    updateStyles();

    Assets::Instance().loadFromFile("bin/editor/config.ini");

    // Register Systems
    m_systems.push_back(std::bind(&Editor::sViewport, this));
    m_systems.push_back(std::bind(&Editor::sUserInput, this));
    m_systems.push_back(std::bind(&Editor::sGUI, this));
}

void Editor::run() {
    while (m_running) { update(); }
}

void Editor::update() {
    ImGui::SFML::Update(window(), m_deltaClock.restart());

    window().clear();

    for (auto system : m_systems) { system(); }

    ImGui::SFML::Render(window());
    m_gameEngine->update();

    window().display();
}

void Editor::quit() {
    m_running = false;
}

void Editor::updateStyles() {
    ImGui::SetupImGuiStyle(m_appState.DarkTheme, 1);
}

void Editor::toggleTheme() {
    m_appState.DarkTheme = !m_appState.DarkTheme;
    updateStyles();
}

sf::RenderWindow& Editor::window() {
    return m_gameEngine->window();
}

void Editor::sViewport() {
    if (m_gameEngine->renderTarget().resize({ (unsigned int)m_viewportSize.x, (unsigned int)m_viewportSize.y })) {}
}

void Editor::sUserInput() {
    while (auto event = window().pollEvent()) {
        ImGui::SFML::ProcessEvent(window(), *event);

        if (event->is<sf::Event::Closed>()) { quit(); }

        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (
                keyPressed->scancode == sf::Keyboard::Scancode::F4 &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl))) {
                quit();
            }
            else if (
                keyPressed->scancode == sf::Keyboard::Scancode::Q &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl)) &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RShift))) {
                quit();
            }
        }

    }
}

void Editor::sGUI() {
    ImGui::DockSpaceOverViewport();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) { /* Handle New action */ }
            if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Handle Open action */ }
            if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) { quit(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Example")) { /* Handle New action */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Toggle Theme")) { toggleTheme(); }
            ImGui::EndMenu();
        }

#ifdef BUILD_EXAMPLES
        if (ImGui::BeginMenu("Examples")) {
            if (ImGui::MenuItem("HelloWorld")) {
                m_gameEngine->changeScene(
                    "HelloWorld",
                    std::make_shared<HelloWorld>(m_gameEngine)
                );
            }

            if (ImGui::MenuItem("GridExample")) {
                m_gameEngine->changeScene(
                    "GridExample",
                    std::make_shared<GridExample>(m_gameEngine)
                );
            }

            if (ImGui::MenuItem("MovingShapes")) {
                Assets::Instance().loadFromFile("bin/moving_shapes/config.ini");
                m_gameEngine->changeScene(
                    "MovingShapes",
                    std::make_shared<MovingShapes>(m_gameEngine)
                );
            }

            if (ImGui::BeginMenu("MegaMario")) {
                if (ImGui::MenuItem("Menu##MegaMario")) {
                    Assets::Instance().loadFromFile("bin/mega_mario/config.ini");
                    m_gameEngine->changeScene(
                        "MegaMario_Menu",
                        std::make_shared<MegaMario_Menu>(m_gameEngine)
                    );
                }
                if (ImGui::MenuItem("Play##MegaMario")) {
                    Assets::Instance().loadFromFile("bin/mega_mario/config.ini");
                    std::string levelPath;
                    m_gameEngine->changeScene(
                        "MegaMario_Play",
                        std::make_shared<MegaMario_Play>(m_gameEngine, levelPath)
                    );
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("ShootEm")) {
                if (ImGui::MenuItem("Menu##ShootEm")) {
                    GameConfig::getInstance().loadFromFile("bin/shoot_em/config.ini");
                    m_gameEngine->changeScene(
                        "ShootEm_Menu",
                        std::make_shared<ShootEm_Menu>(m_gameEngine)
                    );
                }
                if (ImGui::MenuItem("Play##ShootEm")) {
                    GameConfig::getInstance().loadFromFile("bin/shoot_em/config.ini");
                    m_gameEngine->changeScene(
                        "ShootEm_Play",
                        std::make_shared<ShootEm_Play>(m_gameEngine)
                    );
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
#endif

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Example")) { /* Handle New action */ }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Viewport")) {
        m_viewportSize = ImGui::GetWindowSize();
        ImGui::Image(m_gameEngine->renderTarget());
        ImGui::End();
    }

    if (ImGui::Begin("Scene")) {
        if (m_gameEngine->currentScene() && ImGui::TreeNode("Entities")) {
            for (auto& [tag, entities] : m_gameEngine->currentScene()->getEntityManager().getEntityMap()) {
                if (ImGui::TreeNode(tag.c_str())) {
                    for (auto& e : entities) {
                        if (ImGui::Button(std::format("D##{}{}", tag, e->id()).c_str())) {
                            e->destroy();
                        }
                        ImGui::SameLine();
                        ImGui::Text(std::format("{}", e->id()).c_str());
                        ImGui::SameLine();
                        ImGui::Text(std::format("{}", e->tag()).c_str());
                        ImGui::SameLine();
                        if (e->has<CTransform>()) {
                            auto& cTrans = e->get<CTransform>();
                            auto pos = cTrans.pos;
                            ImGui::Text(std::format("({},{})", pos.x, pos.y).c_str());
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }

    if (ImGui::Begin("Assets")) {

        ImGui::End();
    }

    if (ImGui::Begin("Properties")) {

        ImGui::End();
    }

    if (ImGui::Begin("Console")) {

        ImGui::End();
    }
}
