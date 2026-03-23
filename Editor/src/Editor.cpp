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
    m_preSystems.push_back(std::bind(&Editor::sViewport, this));
    m_preSystems.push_back(std::bind(&Editor::sUserInput, this));
    m_postSystems.push_back(std::bind(&Editor::sRender, this));
    m_postSystems.push_back(std::bind(&Editor::sGUI, this));
}

void Editor::run() {
    while (m_running) { update(); }
}

void Editor::update() {
    if (m_gameEngine->currentScene()) {
        m_gameEngine->currentScene()->getEntityManager().update();
    }

    ImGui::SFML::Update(window(), m_deltaClock.restart());

    window().clear();

    // Run systems before game engine update
    for (auto system : m_preSystems) { system(); }

    m_gameEngine->update();

    // Run systems after game engine update
    for (auto system : m_postSystems) { system(); }

    // Render imgui last
    ImGui::SFML::Render(window());

    window().display();
}

void Editor::quit() {
    m_running = false;
}

void Editor::play() {
    if (m_gameEngine->currentScene()) {
        m_gameEngine->currentScene()->setPaused(false);
    }
}

void Editor::pause() {
    if (m_gameEngine->currentScene()) {
        m_gameEngine->currentScene()->setPaused(true);
    }
}

void Editor::stop() {
    if (m_gameEngine->currentScene()) {
        m_gameEngine->currentScene()->setPaused(true);
    }
}

void Editor::updateStyles() {
    ImGui::SetupImGuiStyle(m_appState.DarkTheme, 1);
}

void Editor::toggleTheme() {
    m_appState.DarkTheme = !m_appState.DarkTheme;
    updateStyles();
}

void Editor::toggleGrid() {
    m_appState.DrawGrid = !m_appState.DrawGrid;
}

void Editor::toggleTextures() {
    m_appState.DrawTextures = !m_appState.DrawTextures;
}

void Editor::toggleCollisions() {
    m_appState.DrawCollisions = !m_appState.DrawCollisions;
}

void Editor::toggleAnimationNames() {
    m_appState.DrawAnimationNames = !m_appState.DrawAnimationNames;
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
            else if (keyPressed->scancode == sf::Keyboard::Scancode::F5) {
                play();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::F7) {
                pause();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::F8) {
                stop();
            }
        }

        if (m_gameEngine->currentScene()) m_gameEngine->sHandleEvent(event);
    }
}

void Editor::sRender() {
    auto wWidth = m_gameEngine->renderTarget().getSize().x;
    auto wHeight = m_gameEngine->renderTarget().getSize().y;

    // draw all Entity textures / animations
    if (m_appState.DrawTextures && m_gameEngine->currentScene())
    {
        for (const auto& e : m_gameEngine->currentScene()->getEntityManager().getEntities())
        {
            if (e->has<CAnimation>() && e->has<CTransform>())
            {
                auto& cTrans = e->get<CTransform>();
                auto& anim = e->get<CAnimation>().animation;
                anim.getSprite()->setOrigin(anim.getSprite()->getGlobalBounds().size / 2.f);
                anim.getSprite()->setRotation(sf::radians(cTrans.angle));
                anim.getSprite()->setPosition({ cTrans.pos.x, cTrans.pos.y });
                anim.getSprite()->setScale({ cTrans.scale.x, cTrans.scale.y });
                m_gameEngine->renderTarget().draw(*anim.getSprite());
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_appState.DrawCollisions && m_gameEngine->currentScene())
    {
        for (const auto& e : m_gameEngine->currentScene()->getEntityManager().getEntities())
        {
            if (e->has<CBoundingBox>() && e->has<CTransform>())
            {
                auto& box = e->get<CBoundingBox>();
                auto& cTrans = e->get<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(rect.getGlobalBounds().size / 2.f);
                rect.setPosition({ cTrans.pos.x, cTrans.pos.y });
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1);
                m_gameEngine->renderTarget().draw(rect);
            }
        }
    }

    if (m_appState.DrawAnimationNames && m_gameEngine->currentScene())
    {
        for (const auto& e : m_gameEngine->currentScene()->getEntityManager().getEntities())
        {
            if (e->has<CAnimation>() && e->has<CTransform>())
            {
                auto& cTrans = e->get<CTransform>();
                auto& anim = e->get<CAnimation>().animation;
                sf::Text name(Assets::Instance().getFont("tech"), anim.getName());
                name.setOrigin(name.getGlobalBounds().size / 2.f);
                name.setPosition({ cTrans.pos.x, cTrans.pos.y });
                m_gameEngine->renderTarget().draw(name);
            }
        }
    }

    if (m_appState.DrawGrid)
    {
        sf::Text gridText(Assets::Instance().getFont("tech"), "", 10);
        float leftX = float(m_gameEngine->renderTarget().getView().getCenter().x) - wWidth / 2.0f;
        float rightX = leftX + wWidth + m_gridSize.x;
        float nextGridX = leftX - float((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += float(m_gridSize.x))
        {
            sf::Vertex line[] = {
                sf::Vertex{Vec2f(x, 0)},
                sf::Vertex{Vec2f(x, wHeight)} };

            m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (float y = m_gridSize.y / 2.f; y < wHeight; y += float(m_gridSize.y))
        {
            sf::Vertex line[] = {
                sf::Vertex{Vec2f(leftX, wHeight - y)},
                sf::Vertex{Vec2f(rightX, wHeight - y)} };

            m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);

            for (float x = nextGridX; x < rightX; x += float(m_gridSize.x))
            {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                gridText.setString("(" + xCell + "," + yCell + ")");
                gridText.setPosition({ x + 3, wHeight - y - m_gridSize.y + 2 });
                m_gameEngine->renderTarget().draw(gridText);
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

            if (ImGui::MenuItem("Toggle Textures")) { toggleTextures(); }

            if (ImGui::MenuItem("Toggle Collisions")) { toggleCollisions(); }

            if (ImGui::MenuItem("Toggle Animation Names")) { toggleAnimationNames(); }

            if (ImGui::MenuItem("Toggle Grid")) { toggleGrid(); }

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
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto regionAvail = ImGui::GetContentRegionAvail();
        m_viewportSize = ImGui::GetWindowSize();
        ImGui::Image(m_gameEngine->renderTarget());
        ImGui::End();
    }


    if (ImGui::Begin("Controls")) {
        if (ImGui::Button("Play")) { play(); }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) { pause(); }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) { stop(); }
        ImGui::End();
    }

    if (ImGui::Begin("Hierarchy")) {
        if (m_gameEngine->currentScene()) {
            if (ImGui::Button("Add Entity")) {
                auto e = m_gameEngine->currentScene()->getEntityManager().addEntity("default");
                m_consoleText.append(std::format("[INFO] Added Entity - {}\n", e->id()));
            }

            ImGui::SetNextItemOpen(true);
            if (ImGui::TreeNode("Entities")) {
                for (auto& [tag, entities] : m_gameEngine->currentScene()->getEntityManager().getEntityMap()) {
                    if (ImGui::TreeNode(tag.c_str())) {
                        for (auto& e : entities) {
                            if (ImGui::Button(std::format("D##{}{}", tag, e->id()).c_str())) {
                                e->destroy();
                            }

                            auto name = std::format("{} {}", e->id(), e->tag());
                            if (e->has<CTransform>()) {
                                auto& cTrans = e->get<CTransform>();
                                auto pos = cTrans.pos;
                                name.append(std::format(" ({},{})", pos.x, pos.y));
                            }
                            ImGui::SameLine();
                            if (ImGui::Button(name.c_str())) {
                                m_selectedEntity = e;
                                m_consoleText.append(std::format("[INFO] Selected Entity - {}\n", e->id()));
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }

        ImGui::End(); // ImGui::Begin("Hierarchy")
    }

    if (ImGui::Begin("Inspector")) {
        if (m_selectedEntity) {
            ImGui::Text(std::format("Entity - {}", m_selectedEntity->id()).c_str());
            ImGui::Text(std::format("Tag - {}", m_selectedEntity->tag()).c_str());

            // Build list of available components
            std::vector<const char*> availableNames;
            std::vector<int> availableIndices;
            auto allNames = getComponentNames();

            for (int i = 0; i < getComponentCount(); ++i) {
                if (!hasComponentByEnum(*m_selectedEntity, COMPONENTS[i])) {
                    availableNames.push_back(allNames[i]);
                    availableIndices.push_back(i);
                }
            }

            if (!availableNames.empty()) {
                static int selectedComponent = 0;
                if (ImGui::Combo("##AddComponent", &selectedComponent, availableNames.data(), (int)availableNames.size())) {
                    m_consoleText.append(std::format("[INFO] Selected Component - {}\n", availableNames[selectedComponent]));
                }

                ImGui::SameLine();
                if (ImGui::Button("Add Component")) {
                    int actualIndex = availableIndices[selectedComponent];
                    addComponentByEnum(m_selectedEntity, COMPONENTS[actualIndex]);
                    m_consoleText.append(std::format("[INFO] Added Component - {}\n", availableNames[selectedComponent]));
                    selectedComponent = 0; // Reset selection
                }
            }
            else {
                ImGui::Text("All components added");
            }

            if (m_selectedEntity->has<CTransform>()) {
                auto& cTrans = m_selectedEntity->get<CTransform>();
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    float position[2] = { cTrans.pos.x, cTrans.pos.y };
                    ImGui::InputFloat2("Position", position, "%.1f", 0);
                    cTrans.pos.x = position[0];
                    cTrans.pos.y = position[1];

                    if (ImGui::SliderAngle("Rotation", &cTrans.angle, -180.0f, 180.0f))
                    {
                        // This block is executed only if the user moves the slider.
                        // You can use the updated 'object_angle_rad' value here
                        // to apply the rotation to your 3D model or game object.
                        // e.g., apply_rotation(object_angle_rad);
                    }

                    float scale[2] = { cTrans.scale.x, cTrans.scale.y };
                    ImGui::InputFloat2("Scale", scale, "%.1f", 0);
                    cTrans.scale.x = scale[0];
                    cTrans.scale.y = scale[1];

                    float velocity[2] = { cTrans.velocity.x, cTrans.velocity.y };
                    ImGui::DragFloat2("Velocity", velocity, 0.01f, -1.0f, 1.0f, "%.2f", 0);
                    cTrans.velocity.x = velocity[0];
                    cTrans.velocity.y = velocity[1];

                    ImGui::Unindent();
                }
            }

            if (m_selectedEntity->has<CBoundingBox>()) {
                if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    ImGui::Unindent();
                }
            }

            if (m_selectedEntity->has<CBoundingCircle>()) {
                if (ImGui::CollapsingHeader("Circle Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    ImGui::Unindent();
                }
            }
        }

        ImGui::End(); // ImGui::Begin("Inspector")
    }

    if (ImGui::Begin("Console")) {
        ImGui::Text(m_consoleText.c_str());
        ImGui::End(); // ImGui::Begin("Console")
    }

    if (ImGui::Begin("Project Assets")) {

        if (ImGui::BeginTabBar("Assets")) {
            if (ImGui::BeginTabItem("Textures")) {
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Animations")) {
                auto animations = Assets::Instance().getAnimations();
                int count = 0;
                for (auto& [name, anim] : animations) {
                    count++;
                    ImGui::ImageButton(name.c_str(), *(anim.getSprite()), sf::Vector2f(32, 32));
                    if ((count % 6) != 0 && count != animations.size()) {
                        ImGui::SameLine();
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Fonts")) {
                auto& fonts = Assets::Instance().getFonts();
                for (auto& [name, font_ref] : fonts) {
                    ImGui::Text(std::format("{} - ", name).c_str());
                    ImGui::SameLine();

                    // Create a temporary render texture to display the font
                    try {
                        sf::RenderTexture fontTexture(sf::Vector2u(400, 30));
                        fontTexture.clear(sf::Color::Red);

                        // Create and render text with const reference to font
                        sf::Text displayText(font_ref, "Lorem ipsum dolor sit amet", 20u);
                        displayText.setPosition(sf::Vector2f(5.f, 10.f));
                        displayText.setFillColor(sf::Color::White);

                        fontTexture.draw(displayText);
                        fontTexture.display();

                        // Display the rendered text as an image in ImGui
                        ImGui::Image(fontTexture);
                    }
                    catch (...) {
                        ImGui::Text("(Failed to render font preview)");
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Scripts")) {
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar(); // ImGui::BeginTabBar("Assets")
        }

        ImGui::End(); // ImGui::Begin("Project Assets")
    }
    }


