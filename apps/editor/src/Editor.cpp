#include "Editor.hpp"

#include <algorithm>
#include <format>
#include <memory>
#include <iostream>
#include <optional>

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjs.h>
#endif

#include "imgui.h"
#include "imgui-SFML.h"

#include "TooDeeCore.hpp"
#include "TooDeeEngine.hpp"
#include "bytesize.hpp"

#include "ImGuiComponents.hpp"
#include "ImGuiDirectoryView.hpp"
#include "ImGuiStyles.hpp"

#ifdef BUILD_EXAMPLES
#include "Examples.hpp"
#endif

Editor::Editor(const std::string& configPath) :
    m_gameEngine(std::make_shared<GameEngine>()) {
    m_gameEngine->m_shouldRender = true;
    init(configPath);
}

void Editor::init(const std::string& configPath) {
    std::srand((unsigned int)time(NULL));

    Assets::Instance().loadFromFile(configPath);

    // Initialize Window
    m_gameEngine->window().create(sf::VideoMode::getDesktopMode(), "TooDeeEditor");
    m_gameEngine->window().setFramerateLimit(60);

    // Initialize ImGui
    if (!ImGui::SFML::Init(m_gameEngine->window())) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        exit(1);
    }
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
    updateStyles();

    // Register Systems
    m_preEngineUpdateSystems.push_back(std::bind(&Editor::sViewport, this));
    m_preEngineUpdateSystems.push_back(std::bind(&Editor::sUserInput, this));
    m_postEngineUpdateSystems.push_back(std::bind(&Editor::sMetrics, this));
    m_renderSystems.push_back(std::bind(&Editor::sRender, this));
    m_renderSystems.push_back(std::bind(&Editor::sGUI, this));

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
    m_gameEngine->setupQJSDebug();
    updateQjsStats();
#endif

    reloadScripts();
}

void Editor::run() {
    while (m_running) { update(); }
}

void Editor::update() {
    // Update
    ImGui::SFML::Update(m_gameEngine->window(), m_deltaClock.restart());

    for (auto system : m_preEngineUpdateSystems) { system(); }

    m_gameEngine->update();

    for (auto system : m_postEngineUpdateSystems) { system(); }

    // Render
    m_gameEngine->window().clear();
    Renderer::render(m_gameEngine);
    for (auto system : m_renderSystems) { system(); }
    m_gameEngine->window().display();
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
#ifdef BUILD_EXAMPLES
    if (!m_selectedExample) { return; }

    if (m_gameEngine->currentScene()) {
        unloadExample();
        loadExample(*m_selectedExample);
    }
#endif
}

void Editor::restart() {
    stop();
    play();
}

#ifdef BUILD_EXAMPLES
void Editor::loadExample(Example name) {
    m_selectedExample = name;
    switch (name) {
#ifdef HELLO_WORLD_EXAMPLE
    case Example::HelloWorld: {
        std::string base_path("../../examples/hello_world");
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<HelloWorld>(m_gameEngine);
        m_gameEngine->changeScene("HelloWorld", scene);
        break;
    }
#endif
#ifdef MOVING_SHAPES_EXAMPLE
    case Example::MovingShapes: {
        std::string base_path("../../examples/moving_shapes/");
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<MovingShapes::Example>(m_gameEngine);
        m_gameEngine->changeScene("MovingShapes", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef NATIVE_SCRIPTING_EXAMPLE
    case Example::NativeScripting: {
        std::string base_path("../../examples/native_scripting/");
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<NativeScripting::Example>(m_gameEngine);
        m_gameEngine->changeScene("NativeScripting", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef JAVASCRIPT_SCRIPTING_EXAMPLE
    case Example::JavaScriptScripting: {
        std::string base_path("../../examples/javascript_scripting/");
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<JavaScriptScripting::Example>(m_gameEngine);
        m_gameEngine->changeScene("JavaScriptScripting", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef TYPESCRIPT_SCRIPTING_EXAMPLE
    case Example::TypeScriptScripting: {
        std::string base_path("../../examples/typescript_scripting/");
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<TypeScriptScripting::Example>(m_gameEngine);
        m_gameEngine->changeScene("TypeScriptScripting", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef LUA_SCRIPTING_EXAMPLE
    case Example::LuaScripting: {
        std::string base_path("../../examples/lua_scripting/");
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<LuaScripting::Example>(m_gameEngine);
        m_gameEngine->changeScene("LuaScripting", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
    }

    reloadScripts();
}

void Editor::unloadExample() {
    m_gameEngine->removeScene(m_gameEngine->currentScene());
}
#endif

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
void Editor::updateQjsStats() {
    JSMemoryUsage qjsStats;
    JS_ComputeMemoryUsage(m_gameEngine->m_jsRuntime, &qjsStats);

    m_qjsStats.malloc_size = bytesize::bytesize(std::max(0, (int)qjsStats.malloc_size)).format();
    m_qjsStats.memory_used_size = bytesize::bytesize(std::max(0, (int)qjsStats.memory_used_size)).format();
    m_qjsStats.atom_count = std::to_string(qjsStats.atom_count);
    m_qjsStats.atom_size = bytesize::bytesize(std::max(0, (int)qjsStats.atom_size)).format();
    m_qjsStats.obj_count = std::to_string(qjsStats.obj_count);
    m_qjsStats.obj_size = bytesize::bytesize(std::max(0, (int)qjsStats.obj_size)).format();
    m_qjsStats.str_count = std::to_string(qjsStats.str_count);
    m_qjsStats.str_size = bytesize::bytesize(std::max(0, (int)qjsStats.str_size)).format();
    m_qjsStats.array_count = std::to_string(qjsStats.array_count);
    m_qjsStats.c_func_count = std::to_string(qjsStats.c_func_count);
}
#endif

void Editor::updateStyles() {
    ImGui::SetupImGuiStyle(m_appState.DarkTheme, 1);
}

void Editor::reloadScripts() {
    auto& scripts = Assets::Instance().getScripts();
    std::map<std::string, std::string> scriptPaths;
    for (const auto& [name, script] : scripts) { scriptPaths[name] = script.getPath(); }
    m_scriptsDirectoryTree = createDirectoryNodeTreeFromMap(scriptPaths);
}

bool Editor::shoudPassEventToEngine(std::optional<sf::Event> event) {
    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        return
            !isControlF4(keyPressed) &&
            !isControlShiftQ(keyPressed);
    }
    return true;
}

void Editor::toggleTheme() {
    m_appState.DarkTheme = !m_appState.DarkTheme;
    updateStyles();
}

void Editor::toggleGrid() {
    m_appState.DrawGrid = !m_appState.DrawGrid;
}

void Editor::toggleTextures() {
    m_gameEngine->m_shouldRender = !m_gameEngine->m_shouldRender;
}

void Editor::toggleCollisions() {
    m_appState.DrawCollisions = !m_appState.DrawCollisions;
}

void Editor::toggleAnimationNames() {
    m_appState.DrawAnimationNames = !m_appState.DrawAnimationNames;
}

void Editor::sMetrics() {
#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
    if (m_metricsClock.getElapsedTime().asSeconds() >= 10.0f) {
        updateQjsStats();
        m_metricsClock.restart();
    }
#endif
}

void Editor::sViewport() {
    if (m_gameEngine->renderTarget().resize({ (unsigned int)m_viewportSize.x, (unsigned int)m_viewportSize.y })) {}
}

void Editor::sUserInput() {
    while (auto event = m_gameEngine->window().pollEvent()) {
        ImGui::SFML::ProcessEvent(m_gameEngine->window(), *event);

        if (event->is<sf::Event::Closed>()) { quit(); }

        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (isControlF4(keyPressed) || isControlShiftQ(keyPressed)) {
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

        if (shoudPassEventToEngine(event) && m_gameEngine->currentScene())
            m_gameEngine->handleEvent(event);
    }
}

void Editor::sRender() {
    auto wWidth = m_gameEngine->renderTarget().getSize().x;
    auto wHeight = m_gameEngine->renderTarget().getSize().y;

    if (m_gameEngine->currentScene()) {
        if (m_appState.DrawCollisions) {
            for (const auto& e : m_gameEngine->currentScene()->getEntityManager().getEntities()) {
                if (e->has<CBoundingCircle>() && e->has<CTransform>()) {
                    auto& cTrans = e->get<CTransform>();
                    auto& cCollider = e->get<CBoundingCircle>();

                    auto circle = sf::CircleShape(cCollider.radius - 1);
                    circle.setOrigin(Vec2f(cCollider.radius, cCollider.radius));
                    circle.setPosition(cTrans.pos);
                    circle.setFillColor(sf::Color(0, 0, 0, 0));
                    circle.setOutlineColor(sf::Color::White);
                    circle.setOutlineThickness(1);
                    m_gameEngine->renderTarget().draw(circle);
                }

                if (e->has<CBoundingBox>() && e->has<CTransform>()) {
                    auto& cTrans = e->get<CTransform>();
                    auto& cCollider = e->get<CBoundingBox>();

                    sf::RectangleShape rect;
                    rect.setSize(sf::Vector2f(cCollider.size.x - 1, cCollider.size.y - 1));
                    rect.setOrigin(rect.getGlobalBounds().size / 2.f);
                    rect.setPosition(cTrans.pos);
                    rect.setFillColor(sf::Color(0, 0, 0, 0));
                    rect.setOutlineColor(sf::Color::White);
                    rect.setOutlineThickness(1);
                    m_gameEngine->renderTarget().draw(rect);
                }
            }
        }

        if (m_appState.DrawAnimationNames) {
            for (const auto& e : m_gameEngine->currentScene()->getEntityManager().getEntities()) {
                if (!e->has<CAnimation>() || !e->has<CTransform>()) { continue; }

                auto& cTrans = e->get<CTransform>();
                auto& anim = e->get<CAnimation>().animation;
                sf::Text name(Assets::Instance().getFont("tech"), anim.getName());
                name.setOrigin(name.getGlobalBounds().size / 2.f);
                name.setPosition({ cTrans.pos.x, cTrans.pos.y });
                m_gameEngine->renderTarget().draw(name);

            }
        }
    }

    if (m_appState.DrawGrid) {
        sf::Text gridText(Assets::Instance().getFont("tech"), "", 10);
        float leftX = float(m_gameEngine->renderTarget().getView().getCenter().x) - wWidth / 2.0f;
        float rightX = leftX + wWidth + m_gridSize.x;
        float nextGridX = leftX - float((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += float(m_gridSize.x)) {
            sf::Vertex line[] = {
                sf::Vertex{Vec2f(x, 0)},
                sf::Vertex{Vec2f(x, wHeight)} };

            m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (float y = m_gridSize.y; y < wHeight; y += float(m_gridSize.y)) {
            sf::Vertex line[] = {
                sf::Vertex{Vec2f(leftX, wHeight - y)},
                sf::Vertex{Vec2f(rightX, wHeight - y)} };

            m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);

            for (float x = nextGridX; x < rightX; x += float(m_gridSize.x)) {
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

            if (ImGui::MenuItem("Textures")) { toggleTextures(); }

            if (ImGui::MenuItem("Collisions")) { toggleCollisions(); }

            if (ImGui::MenuItem("Animation Names")) { toggleAnimationNames(); }

            if (ImGui::MenuItem("Grid")) { toggleGrid(); }

            ImGui::Separator();

            if (ImGui::MenuItem("Toggle Theme")) { toggleTheme(); }

            ImGui::EndMenu();
        }

#ifdef BUILD_EXAMPLES
        if (ImGui::BeginMenu("Examples")) {
#ifdef HELLO_WORLD_EXAMPLE
            if (ImGui::MenuItem("Hello World")) {
                loadExample(Example::HelloWorld);
            }
#endif

#ifdef MOVING_SHAPES_EXAMPLE
            if (ImGui::MenuItem("Moving Shapes")) {
                loadExample(Example::MovingShapes);
            }
#endif

#ifdef NATIVE_SCRIPTING_EXAMPLE
            if (ImGui::MenuItem("Native Scripting")) {
                loadExample(Example::NativeScripting);
            }
#endif

#ifdef JAVASCRIPT_SCRIPTING_EXAMPLE
            if (ImGui::MenuItem("JavaScript Scripting")) {
                loadExample(Example::JavaScriptScripting);
            }
#endif

#ifdef TYPESCRIPT_SCRIPTING_EXAMPLE
            if (ImGui::MenuItem("TypeScript Scripting")) {
                loadExample(Example::TypeScriptScripting);
            }
#endif

#ifdef LUA_SCRIPTING_EXAMPLE
            if (ImGui::MenuItem("Lua Scripting")) {
                loadExample(Example::LuaScripting);
            }
#endif
            ImGui::EndMenu();
        }
#endif

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) { /* Handle New action */ }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (ImGui::Begin("Viewport")) {
        auto wSize = ImGui::GetWindowSize();
        m_viewportSize = ImVec2(wSize.x, wSize.y - ImGui::GetFrameHeight());
        ImGui::Image(m_gameEngine->renderTarget());
        ImGui::PopStyleVar();
        ImGui::End();
    }

    if (ImGui::Begin("Controls")) {
        bool hasScene = m_gameEngine->currentScene() != nullptr;
        bool isPaused = hasScene && m_gameEngine->currentScene()->isPaused();
        if (ImGui::IconButton("Play", "play_icon", hasScene && isPaused)) { play(); }
        ImGui::SameLine();
        if (ImGui::IconButton("Pause", "pause_icon", hasScene && !isPaused)) { pause(); }
        ImGui::SameLine();
        if (ImGui::IconButton("Restart", "retry_icon", hasScene)) { restart(); }
        ImGui::SameLine();
        if (ImGui::IconButton("Stop", "stop_icon", hasScene)) { stop(); }
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
                            if (ImGui::IconButton(std::format("D##{}{}", tag, e->id()).c_str(), "bin_icon")) {
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

            if (m_selectedEntity->has<CRectangle>()) {
                auto& cRect = m_selectedEntity->get<CRectangle>();
                if (ImGui::CollapsingHeader("Rectangle", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    float size[2] = { cRect.size.x, cRect.size.y };
                    ImGui::InputFloat2("Size##Rectangle", size, "%.1f", 0);
                    cRect.size.x = size[0];
                    cRect.size.y = size[1];

                    ImGui::Unindent();
                }
            }

            if (m_selectedEntity->has<CCircle>()) {
                auto& cRect = m_selectedEntity->get<CCircle>();
                if (ImGui::CollapsingHeader("Circle", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    if (ImGui::DragFloat("Radius##Circle", &cRect.radius, 1.0f, 0.0f)) {
                        if (cRect.radius < 0.0f) cRect.radius = 0.0f;
                    }

                    ImGui::Unindent();
                }

            }

            if (m_selectedEntity->has<CBoundingBox>()) {
                auto& cBound = m_selectedEntity->get<CBoundingBox>();
                if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    float size[2] = { cBound.size.x, cBound.size.y };
                    ImGui::InputFloat2("Size##BoxCollider", size, "%.1f", 0);
                    cBound.size.x = size[0];
                    cBound.size.y = size[1];

                    for (auto& e : m_gameEngine->currentScene()->getEntityManager().getEntities()) {
                        if (e == m_selectedEntity) { continue; }

                        if (e->has<CBoundingBox>()) {
                            Vec2f overlap = Physics::GetOverlap(m_selectedEntity, e);
                            if (overlap.x >= 0 && overlap.y >= 0) {
                                if (ImGui::Button(std::format("{} - {}", e->tag(), e->id()).c_str())) {}
                            }
                        }
                    }

                    ImGui::Unindent();
                }
            }

            if (m_selectedEntity->has<CBoundingCircle>()) {
                auto& cBound = m_selectedEntity->get<CBoundingCircle>();
                if (ImGui::CollapsingHeader("Circle Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    if (ImGui::DragFloat("Radius##CircleCollider", &cBound.radius, 1.0f, 0.0f)) {
                        if (cBound.radius < 0.0f) cBound.radius = 0.0f;
                    }

                    ImGui::Unindent();
                }
            }

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
            if (m_selectedEntity->has<CQJSScript>()) {
                auto& cJSScript = m_selectedEntity->get<CQJSScript>();
                auto& scriptPath = Assets::Instance().getScript(cJSScript.name).getPath();
                if (ImGui::CollapsingHeader("JavaScript Script", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent();

                    ImGui::Text("Name: %s", cJSScript.name.c_str());
                    ImGui::Text("Path: %s", scriptPath.c_str());
                    if (ImGui::Button("Open Script")) {
                        openFile(std::filesystem::current_path() / scriptPath);
                    }

                    ImGui::Unindent();
                }
            }
#endif

        }

        ImGui::End(); // ImGui::Begin("Inspector")
    }

    if (ImGui::Begin("Console")) {
        ImGui::Text(m_consoleText.c_str());
        ImGui::End(); // ImGui::Begin("Console")
    }

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
    if (ImGui::Begin("Memory Stats")) {
        ImGui::Text("Malloc size: %s", m_qjsStats.malloc_size.c_str());
        ImGui::Text("Memory used: %s", m_qjsStats.memory_used_size.c_str());
        ImGui::Text("Atoms: %s (%s)", m_qjsStats.atom_count.c_str(), m_qjsStats.atom_size.c_str());
        ImGui::Text("Objects: %s (%s)", m_qjsStats.obj_count.c_str(), m_qjsStats.obj_size.c_str());
        ImGui::Text("Strings: %s (%s)", m_qjsStats.str_count.c_str(), m_qjsStats.str_size.c_str());
        ImGui::Text("Arrays: %s", m_qjsStats.array_count.c_str());
        ImGui::Text("C functions: %s", m_qjsStats.c_func_count.c_str());
        ImGui::End();
    }
#endif

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
                    auto& texture = Assets::Instance().getTexture(anim.getTextureName());
                    auto sprite = sf::Sprite(texture, anim.getSpriteRect());
                    ImGui::ImageButton(name.c_str(), sprite, sf::Vector2f(32, 32));
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
                        displayText.setFillColor(sf::Color::Blue);

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
                ImGuiDirectoryNode(m_scriptsDirectoryTree);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar(); // ImGui::BeginTabBar("Assets")
        }

        ImGui::End(); // ImGui::Begin("Project Assets")
    }

    ImGui::SFML::Render(m_gameEngine->window());
}

bool isControlF4(const sf::Event::KeyPressed* keyPressed) {
    bool isControl =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl);
    return isControl && keyPressed->scancode == sf::Keyboard::Scancode::F4;
}

bool isControlShiftQ(const sf::Event::KeyPressed* keyPressed) {
    bool isControl =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl);
    bool isShift =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RShift);
    return isControl && isShift && keyPressed->scancode == sf::Keyboard::Scancode::Q;
}
