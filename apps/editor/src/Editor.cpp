#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#endif

#include "Editor.hpp"

#include <algorithm>
#include <cctype>
#include <format>
#include <memory>
#include <iostream>
#include <optional>
#include <fstream>
#include <set>
#include <cwchar>

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include "quickjs.h"
#endif

#include "spdlog/spdlog.h"
#include "imgui.h"
#include "imgui-SFML.h"

#include "TooDeeCore.hpp"
#include "TooDeeEngine.hpp"
#include "bytesize.hpp"

#include "ImGuiComponents.hpp"
#include "ImGuiDirectoryView.hpp"
#include "ImGuiStyles.hpp"
#include "INIReader.h"

#ifdef BUILD_EXAMPLES
#include "Examples.hpp"
#endif

namespace
{
#ifdef _WIN32
    constexpr UINT_PTR FileDropSubclassId = 0x544445;

    LRESULT CALLBACK fileDropWindowProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR referenceData) {
        if (message == WM_DROPFILES) {
            const HDROP dropHandle = reinterpret_cast<HDROP>(wParam);
            const UINT fileCount = DragQueryFileW(dropHandle, 0xFFFFFFFF, nullptr, 0);
            std::vector<std::filesystem::path> paths;
            paths.reserve(fileCount);

            for (UINT index = 0; index < fileCount; ++index) {
                const UINT pathLength = DragQueryFileW(dropHandle, index, nullptr, 0);
                std::wstring path(pathLength + 1, L'\0');
                if (DragQueryFileW(dropHandle, index, path.data(), pathLength + 1) > 0) {
                    path.resize(pathLength);
                    paths.emplace_back(path);
                }
            }

            POINT point{};
            DragQueryPoint(dropHandle, &point);
            DragFinish(dropHandle);

            if (auto* editor = reinterpret_cast<Editor*>(referenceData)) {
                editor->queueDroppedFiles(std::move(paths), ImVec2(
                    static_cast<float>(point.x),
                    static_cast<float>(point.y)));
            }
            return 0;
        }

        return DefSubclassProc(window, message, wParam, lParam);
    }
#endif

    std::string assetTypeForExtension(const std::filesystem::path& path) {
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char character) { return static_cast<char>(std::tolower(character)); });

        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
            extension == ".bmp" || extension == ".tga") {
            return "texture";
        }
        if (extension == ".ttf" || extension == ".otf") {
            return "font";
        }
        if (extension == ".js" || extension == ".lua") {
            return "script";
        }
        return {};
    }

    std::string makeAssetName(const std::filesystem::path& path) {
        std::string name = path.stem().string();
        std::transform(name.begin(), name.end(), name.begin(), [](unsigned char character) {
            return std::isalnum(character) || character == '_' || character == '-'
                ? static_cast<char>(character)
                : '_';
        });
        return name.empty() ? "asset" : name;
    }

#ifdef _WIN32
    std::vector<std::filesystem::path> selectAssetFiles(HWND owner) {
        std::vector<wchar_t> selectedPaths(32768, L'\0');
        const wchar_t filter[] =
            L"Supported assets\0*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.ttf;*.otf;*.js;*.lua\0"
            L"All files\0*.*\0\0";

        OPENFILENAMEW dialog{};
        dialog.lStructSize = sizeof(dialog);
        dialog.hwndOwner = owner;
        dialog.lpstrFilter = filter;
        dialog.lpstrFile = selectedPaths.data();
        dialog.nMaxFile = static_cast<DWORD>(selectedPaths.size());
        dialog.lpstrTitle = L"Add Assets";
        dialog.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST |
            OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

        if (!GetOpenFileNameW(&dialog)) {
            const DWORD error = CommDlgExtendedError();
            if (error != 0) {
                spdlog::error("Asset file picker failed with error {}", error);
            }
            return {};
        }

        const std::wstring firstPath(selectedPaths.data());
        const wchar_t* next = selectedPaths.data() + firstPath.size() + 1;
        std::vector<std::filesystem::path> paths;
        if (*next == L'\0') {
            paths.emplace_back(firstPath);
            return paths;
        }

        for (; *next != L'\0'; next += std::wcslen(next) + 1) {
            paths.emplace_back(std::filesystem::path(firstPath) / next);
        }
        return paths;
    }
#endif
}

Editor::Editor(const std::string& configPath) :
    m_gameEngine(std::make_shared<GameEngine>()) {
    m_gameEngine->m_shouldRender = true;
    init(configPath);
}

Editor::~Editor() {
#ifdef _WIN32
    if (m_nativeWindowHandle != 0) {
        const auto window = reinterpret_cast<HWND>(m_nativeWindowHandle);
        DragAcceptFiles(window, FALSE);
        RemoveWindowSubclass(window, fileDropWindowProc, FileDropSubclassId);
    }
#endif
}

void Editor::init(const std::string& configPath) {
    std::srand((unsigned int)time(NULL));

    m_activeConfigBase = std::filesystem::current_path();
    m_activeConfigPath = std::filesystem::absolute(m_activeConfigBase / configPath).lexically_normal();
    m_assetRoot = (m_activeConfigBase / "assets").lexically_normal();
    Assets::Instance().loadFromFile(m_activeConfigPath.filename(), m_activeConfigBase);

    // Set DEBUG Logger

    // Initialize Window
    m_gameEngine->window().create(sf::VideoMode::getDesktopMode(), "TooDeeEditor");
    m_gameEngine->window().setFramerateLimit(60);

#ifdef _WIN32
    const auto nativeWindow = static_cast<HWND>(m_gameEngine->window().getNativeHandle());
    m_nativeWindowHandle = reinterpret_cast<std::uintptr_t>(nativeWindow);
    if (!SetWindowSubclass(nativeWindow, fileDropWindowProc, FileDropSubclassId, reinterpret_cast<DWORD_PTR>(this))) {
        spdlog::error("Failed to enable file drops for the project asset browser");
    }
    else {
        DragAcceptFiles(nativeWindow, TRUE);
    }
#endif

    sf::Image icon = Assets::Instance().getTexture("too_dee_icon").copyToImage();
    m_gameEngine->window().setIcon(icon);

    // Initialize ImGui
    if (!ImGui::SFML::Init(m_gameEngine->window())) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        exit(1);
    }
    ImGuiIO& imguiIO = ImGui::GetIO();
    imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imguiIO.FontGlobalScale = 1.2f;
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
    auto setAssetConfig = [this](const std::filesystem::path& basePath) {
        m_activeConfigBase = std::filesystem::absolute(basePath).lexically_normal();
        m_activeConfigPath = m_activeConfigBase / "config.ini";
        m_assetRoot = m_activeConfigBase / "assets";
    };
    switch (name) {
#ifdef HELLO_WORLD_EXAMPLE
    case Example::HelloWorld: {
        std::string base_path("../../examples/hello_world");
        setAssetConfig(base_path);
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<HelloWorld>(m_gameEngine);
        m_gameEngine->changeScene("HelloWorld", scene);
        break;
    }
#endif
#ifdef MOVING_SHAPES_EXAMPLE
    case Example::MovingShapes: {
        spdlog::debug("Loading Moving Shapes example");
        std::string base_path("../../examples/moving_shapes/");
        setAssetConfig(base_path);
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<MovingShapes::Example>(m_gameEngine);
        m_gameEngine->changeScene("MovingShapes", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef NATIVE_SCRIPTING_EXAMPLE
    case Example::NativeScripting: {
        spdlog::debug("Loading Native/C++ scripting example");
        std::string base_path("../../examples/native_scripting/");
        setAssetConfig(base_path);
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<NativeScripting::Example>(m_gameEngine);
        m_gameEngine->changeScene("NativeScripting", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef JAVASCRIPT_SCRIPTING_EXAMPLE
    case Example::JavaScriptScripting: {
        spdlog::debug("Loading JavaScript scripting example");
        std::string base_path("../../examples/javascript_scripting/");
        setAssetConfig(base_path);
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<JavaScriptScripting::Example>(m_gameEngine);
        m_gameEngine->changeScene("JavaScriptScripting", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef TYPESCRIPT_SCRIPTING_EXAMPLE
    case Example::TypeScriptScripting: {
        spdlog::debug("Loading TypeScript scripting example");
        std::string base_path("../../examples/typescript_scripting/");
        setAssetConfig(base_path);
        Assets::Instance().loadFromFile("config.ini", base_path);
        auto scene = std::make_shared<TypeScriptScripting::Example>(m_gameEngine);
        m_gameEngine->changeScene("TypeScriptScripting", scene);
        scene->loadLevel(base_path + "levels/level.txt");
        break;
    }
#endif
#ifdef LUA_SCRIPTING_EXAMPLE
    case Example::LuaScripting: {
        spdlog::debug("Loading Lua scripting example");
        std::string base_path("../../examples/lua_scripting/");
        setAssetConfig(base_path);
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
    ImGui::SetupImGuiStyle(m_appState.DarkTheme);
}

void Editor::reloadScripts() {
    m_assetDirectoryTree = createDirectoryNodeTreeFromPath(m_assetRoot);
}

void Editor::queueDroppedFiles(
    std::vector<std::filesystem::path> paths,
    const ImVec2& dropPosition) {
    m_pendingDroppedFiles = std::move(paths);
    m_pendingDropPosition = dropPosition;
}

void Editor::importDroppedFiles(const std::filesystem::path& destination) {
    if (m_pendingDroppedFiles.empty()) {
        return;
    }

    std::error_code pathError;
    const auto assetRoot = std::filesystem::weakly_canonical(m_assetRoot, pathError);
    pathError.clear();
    const auto dropDirectory = std::filesystem::weakly_canonical(destination, pathError);
    if (pathError || !std::filesystem::is_directory(dropDirectory)) {
        spdlog::error("Cannot import files: invalid asset drop directory '{}'", destination.string());
        m_logger << "[ERROR] Cannot import files: invalid asset drop directory" << std::endl;
        m_pendingDroppedFiles.clear();
        m_pendingDropPosition = ImVec2(-1.0f, -1.0f);
        return;
    }

    const auto relativeDropPath = dropDirectory.lexically_relative(assetRoot);
    if (relativeDropPath.empty() || *relativeDropPath.begin() == "..") {
        spdlog::error("Cannot import files outside the project asset directory");
        m_logger << "[ERROR] Cannot import files outside the project asset directory" << std::endl;
        m_pendingDroppedFiles.clear();
        m_pendingDropPosition = ImVec2(-1.0f, -1.0f);
        return;
    }

    INIReader reader(m_activeConfigPath.string());
    std::set<std::string> existingSections;
    for (const auto& section : reader.Sections()) {
        existingSections.insert(section);
    }

    bool reloadAssets = false;
    for (const auto& source : m_pendingDroppedFiles) {
        std::error_code error;
        if (!std::filesystem::is_regular_file(source, error) || error) {
            spdlog::warn("Skipping dropped item because it is not a regular file: {}", source.string());
            m_logger << std::format("[WARN] Skipped non-file: {}", source.filename().string()) << std::endl;
            continue;
        }

        const auto type = assetTypeForExtension(source);
        std::filesystem::path target = dropDirectory / source.filename();
        for (unsigned int suffix = 1; std::filesystem::exists(target, error); ++suffix) {
            target = dropDirectory / std::format("{} ({}){}",
                source.stem().string(), suffix, source.extension().string());
        }

        if (!std::filesystem::copy_file(source, target, std::filesystem::copy_options::none, error)) {
            spdlog::error("Failed to copy dropped asset '{}' to '{}': {}",
                source.string(), target.string(), error.message());
            m_logger << std::format("[ERROR] Failed to copy asset: {}", source.filename().string()) << std::endl;
            continue;
        }

        if (type.empty()) {
            spdlog::info("Copied unsupported asset type to project assets: {}", target.string());
            m_logger << std::format("[INFO] Copied file (not registered): {}", target.filename().string()) << std::endl;
            continue;
        }

        const auto relativePath = target.lexically_relative(m_activeConfigBase).generic_string();
        const auto baseName = makeAssetName(target);
        std::string name = baseName;
        for (unsigned int suffix = 1; existingSections.contains(type + "." + name); ++suffix) {
            name = std::format("{}_{}", baseName, suffix);
        }
        const auto section = type + "." + name;

        std::ofstream config(m_activeConfigPath, std::ios::app);
        if (!config) {
            spdlog::error("Failed to open asset config for writing: {}", m_activeConfigPath.string());
            m_logger << "[ERROR] Copied file but could not update the asset config" << std::endl;
            continue;
        }
        config << "\n[" << section << "]\nfile=" << relativePath << '\n';
        config.flush();
        if (!config) {
            spdlog::error("Failed to write asset entry '{}' to '{}'", section, m_activeConfigPath.string());
            m_logger << "[ERROR] Copied file but failed to write its asset config entry" << std::endl;
            continue;
        }

        existingSections.insert(section);
        reloadAssets = true;
        spdlog::info("Imported {} asset '{}' from '{}'", type, name, target.string());
        m_logger << std::format("[INFO] Imported {}: {}", type, target.filename().string()) << std::endl;
    }

    if (reloadAssets) {
        Assets::Instance().loadFromFile(
            m_activeConfigPath.lexically_relative(m_activeConfigBase),
            m_activeConfigBase);
    }
    reloadScripts();
    m_pendingDroppedFiles.clear();
    m_pendingDropPosition = ImVec2(-1.0f, -1.0f);
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
    if (m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f) {
        const sf::Vector2u viewportSize(
            static_cast<unsigned int>(m_viewportSize.x),
            static_cast<unsigned int>(m_viewportSize.y));
        if (viewportSize != m_gameEngine->renderTarget().getSize() &&
            !m_gameEngine->renderTarget().resize(viewportSize)) {
            spdlog::warn("Failed to resize scene viewport to {}x{}", viewportSize.x, viewportSize.y);
        }
    }
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
        float leftX = float(m_gameEngine->renderTarget().getView().getCenter().x) - wWidth / 2.0f;
        float rightX = leftX + wWidth + m_gridSize.x;
        float nextGridX = leftX - float((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += float(m_gridSize.x)) {
            sf::Vertex line[] = {
                sf::Vertex{ Vec2f(x, 0) },
                sf::Vertex{ Vec2f(x, wHeight) } };

            m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (float y = m_gridSize.y; y < wHeight; y += float(m_gridSize.y)) {
            sf::Vertex line[] = {
                sf::Vertex{ Vec2f(leftX, wHeight - y) },
                sf::Vertex{ Vec2f(rightX, wHeight - y) } };

            m_gameEngine->renderTarget().draw(line, 2, sf::PrimitiveType::Lines);

        }

        sf::Text gridText(Assets::Instance().getFont("tech"), "", 10);
        for (float y = 0; y < wHeight; y += float(m_gridSize.y)) {
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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
    if (ImGui::Begin("Viewport")) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("SCENE VIEW");
        ImGui::SameLine();
        const bool hasScene = m_gameEngine->currentScene() != nullptr;
        const bool isPaused = hasScene && m_gameEngine->currentScene()->isPaused();
        ImGui::Checkbox("Grid", &m_appState.DrawGrid);
        ImGui::SameLine();
        ImGui::Checkbox("Collisions", &m_appState.DrawCollisions);
        ImGui::SameLine();
        ImGui::Checkbox("Names", &m_appState.DrawAnimationNames);
        ImGui::SameLine();
        ImGui::Checkbox("Textures", &m_gameEngine->m_shouldRender);
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        if (ImGui::IconButton("PlayPause", isPaused ? "play_icon" : "pause_icon", hasScene)) {
            if (isPaused) { play(); }
            else { pause(); }
        }
        ImGui::SameLine();
        if (ImGui::IconButton("Stop", "stop_icon", hasScene)) { stop(); }
        ImGui::SameLine();
        ImGui::TextDisabled(m_gameEngine->currentScene() ? "SCENE LOADED" : "NO SCENE");
        ImGui::Separator();

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_viewportSize = ImVec2(std::max(0.0f, viewportSize.x), std::max(0.0f, viewportSize.y));
        ImGui::Image(m_gameEngine->renderTarget(), m_viewportSize);
    }
    ImGui::End();
    ImGui::PopStyleVar();

    if (ImGui::Begin("Hierarchy")) {
        if (ImGui::Button("+##AddEntity", ImVec2(30.0f, 0.0f)) && m_gameEngine->currentScene()) {
            auto entity = m_gameEngine->currentScene()->getEntityManager().addEntity("default");
            m_logger << std::format("[INFO] Added Entity - {}\n", entity->id()) << std::endl;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##HierarchySearch", "Search entities...", m_hierarchySearch, sizeof(m_hierarchySearch));

        if (m_gameEngine->currentScene()) {
            const std::string query = m_hierarchySearch;
            auto matchesQuery = [&query](std::string value) {
                auto lowercase = [](unsigned char character) { return static_cast<char>(std::tolower(character)); };
                std::transform(value.begin(), value.end(), value.begin(), lowercase);
                std::string loweredQuery = query;
                std::transform(loweredQuery.begin(), loweredQuery.end(), loweredQuery.begin(), lowercase);
                return loweredQuery.empty() || value.find(loweredQuery) != std::string::npos;
            };
            ImGui::SetNextItemOpen(true);
            if (ImGui::TreeNode("Entities")) {
                for (auto& [tag, entities] : m_gameEngine->currentScene()->getEntityManager().getEntityMap()) {
                    std::vector<std::pair<std::shared_ptr<Entity>, std::string>> visibleEntities;
                    for (const auto& entity : entities) {
                        auto name = std::format("{} {}", entity->id(), entity->tag());
                        if (entity->has<CTransform>()) {
                            const auto& position = entity->get<CTransform>().pos;
                            name.append(std::format(" ({},{})", position.x, position.y));
                        }
                        if (matchesQuery(tag) || matchesQuery(name)) {
                            visibleEntities.emplace_back(entity, std::move(name));
                        }
                    }

                    if (!visibleEntities.empty() && ImGui::TreeNode(tag.c_str())) {
                        for (const auto& [entity, name] : visibleEntities) {
                            if (ImGui::IconButton(std::format("D##{}{}", tag, entity->id()).c_str(), "bin_icon")) {
                                entity->destroy();
                            }
                            ImGui::SameLine();
                            if (ImGui::Selectable(name.c_str(), m_selectedEntity == entity)) {
                                m_selectedEntity = entity;
                                m_logger << std::format("[INFO] Selected Entity - {}", entity->id()) << std::endl;
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }

    }
    ImGui::End();

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
                    m_logger << std::format("[INFO] Selected Component - {}", availableNames[selectedComponent]) << std::endl;
                }

                ImGui::SameLine();
                if (ImGui::Button("Add Component")) {
                    int actualIndex = availableIndices[selectedComponent];
                    addComponentByEnum(m_selectedEntity, COMPONENTS[actualIndex]);
                    m_logger << std::format("[INFO] Added Component - {}", availableNames[selectedComponent]) << std::endl;
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
    }
    ImGui::End();

    if (ImGui::Begin("Console")) {
        ImGui::Text(m_logger.str().c_str());
    }
    ImGui::End();

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
    if (ImGui::Begin("Memory Stats")) {
        ImGui::Text("Malloc size: %s", m_qjsStats.malloc_size.c_str());
        ImGui::Text("Memory used: %s", m_qjsStats.memory_used_size.c_str());
        ImGui::Text("Atoms: %s (%s)", m_qjsStats.atom_count.c_str(), m_qjsStats.atom_size.c_str());
        ImGui::Text("Objects: %s (%s)", m_qjsStats.obj_count.c_str(), m_qjsStats.obj_size.c_str());
        ImGui::Text("Strings: %s (%s)", m_qjsStats.str_count.c_str(), m_qjsStats.str_size.c_str());
        ImGui::Text("Arrays: %s", m_qjsStats.array_count.c_str());
        ImGui::Text("C functions: %s", m_qjsStats.c_func_count.c_str());
    }
    ImGui::End();
#endif

    std::filesystem::path dropTarget = m_assetRoot;
    std::optional<ImVec2> dropPosition;
    if (!m_pendingDroppedFiles.empty() &&
        m_pendingDropPosition.x >= 0.0f && m_pendingDropPosition.y >= 0.0f) {
        dropPosition = m_pendingDropPosition;
    }

    if (ImGui::Begin("Project Assets")) {
        static const char* assetTypeNames[] = { "All Types", "Textures", "Fonts", "Scripts", "Other" };
        if (ImGui::Button("+##AddAsset", ImVec2(30.0f, 0.0f))) {
#ifdef _WIN32
            auto selectedFiles = selectAssetFiles(reinterpret_cast<HWND>(m_nativeWindowHandle));
            if (!selectedFiles.empty()) {
                queueDroppedFiles(std::move(selectedFiles), ImVec2(-1.0f, -1.0f));
            }
#else
            spdlog::warn("Adding files through the asset picker is only supported on Windows");
            m_logger << "[WARN] Asset file picker is not supported on this platform" << std::endl;
#endif
        }
        ImGui::SameLine();
        const float availableWidth = ImGui::GetContentRegionAvail().x;
        const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float filterWidth = std::min(120.0f, std::max(70.0f, availableWidth * 0.34f));
        const float searchWidth = std::max(40.0f, availableWidth - filterWidth - itemSpacing);
        ImGui::SetNextItemWidth(searchWidth);
        ImGui::InputTextWithHint("##AssetSearch", "Search assets...", m_assetSearch, sizeof(m_assetSearch));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(filterWidth);
        ImGui::Combo("##AssetType", &m_assetTypeFilter, assetTypeNames, IM_ARRAYSIZE(assetTypeNames));
        ImGui::TextDisabled("Drop files here or onto a folder to import them");
        ImGui::Separator();

        if (ImGui::BeginChild("AssetTree", ImVec2(0.0f, 0.0f), false)) {
            ImGuiAssetDirectoryNode(
                m_assetDirectoryTree,
                m_assetSearch,
                m_assetTypeFilter,
                dropPosition,
                dropTarget);
        }
        ImGui::EndChild();
    }
    ImGui::End();

    if (!m_pendingDroppedFiles.empty()) {
        importDroppedFiles(dropTarget);
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
