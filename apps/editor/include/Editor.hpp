#pragma once

#include <memory>
#include <functional>
#include <optional>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "imgui.h"

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include "quickjs.h"
#endif

#include "ImGuiDirectoryView.hpp"
#include "TooDeeCore.hpp"
#include "TooDeeEngine.hpp"


#ifdef BUILD_EXAMPLES
enum class Example {
#ifdef HELLO_WORLD_EXAMPLE
    HelloWorld,
#endif
#ifdef MOVING_SHAPES_EXAMPLE
    MovingShapes,
#endif
#ifdef NATIVE_SCRIPTING_EXAMPLE
    NativeScripting,
#endif
#ifdef JAVASCRIPT_SCRIPTING_EXAMPLE
    JavaScriptScripting,
#endif
#ifdef TYPESCRIPT_SCRIPTING_EXAMPLE
    TypeScriptScripting,
#endif
#ifdef LUA_SCRIPTING_EXAMPLE
    LuaScripting,
#endif
};
#endif

struct AppState {
    bool DarkTheme = true;
    bool DrawGrid = false;
    bool DrawCollisions = false;
    bool DrawAnimationNames = false;
};

struct MenuState {
    // Examples Apps (accessible from the "Examples" menu)
    bool ShowMainMenuBar = false;
    bool ShowAppAssetsBrowser = false;
    bool ShowAppConsole = false;
    bool ShowAppCustomRendering = false;
    bool ShowAppDocuments = false;
    bool ShowAppLog = false;
    bool ShowAppLayout = false;
    bool ShowAppPropertyEditor = false;
    bool ShowAppSimpleOverlay = false;
    bool ShowAppAutoResize = false;
    bool ShowAppConstrainedResize = false;
    bool ShowAppFullscreen = false;
    bool ShowAppLongText = false;
    bool ShowAppWindowTitles = false;

    // Dear ImGui Tools (accessible from the "Tools" menu)
    bool ShowMetrics = false;
    bool ShowDebugLog = false;
    bool ShowIDStackTool = false;
    bool ShowStyleEditor = false;
    bool ShowAbout = false;

    // Other data
    bool DisableSections = false;
};

struct JSMemoryUsageDisplay {
    std::string malloc_size = "";
    std::string memory_used_size = "";
    std::string atom_count = "";
    std::string atom_size = "";
    std::string obj_count = "";
    std::string obj_size = "";
    std::string str_count = "";
    std::string str_size = "";
    std::string array_count = "";
    std::string c_func_count = "";
};

bool isControlF4(const sf::Event::KeyPressed*);

bool isControlShiftQ(const sf::Event::KeyPressed*);

class Editor {
    bool                        m_running = true;

    AppState                    m_appState;
    MenuState                   m_menuState;

    sf::Clock                   m_deltaClock;
    sf::Clock                   m_metricsClock;
    std::shared_ptr<GameEngine> m_gameEngine;
    ImVec2                      m_viewportSize;

#ifdef BUILD_EXAMPLES
    std::optional<Example>      m_selectedExample;
#endif

    std::vector<std::function<void()>> m_preEngineUpdateSystems;
    std::vector<std::function<void()>> m_postEngineUpdateSystems;
    std::vector<std::function<void()>> m_renderSystems;

    std::stringstream   m_logger;
    DirectoryNode       m_scriptsDirectoryTree;

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
    JSMemoryUsageDisplay m_qjsStats;
#endif

    std::shared_ptr<Entity> m_selectedEntity;

    const Vec2f m_gridSize = { 64, 64 };

    void init(const std::string& configPath);

public:
    Editor(const std::string& configPath);

    void run();
    void update();
    void quit();

    void play();
    void pause();
    void stop();
    void restart();

#ifdef BUILD_EXAMPLES
    void loadExample(Example name);
    void unloadExample();
#endif
#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
    void updateQjsStats();
#endif
    void updateStyles();
    void reloadScripts();
    bool shoudPassEventToEngine(std::optional<sf::Event> event);
    void toggleTheme();
    void toggleGrid();
    void toggleTextures();
    void toggleCollisions();
    void toggleAnimationNames();

    void sMetrics();
    void sViewport();
    void sUserInput();
    void sRender();
    void sGUI();
};
