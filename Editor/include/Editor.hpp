#pragma once

#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "GameEngine.hpp"

struct AppState {
    bool DarkTheme = true;
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

class Editor {
    sf::Clock m_deltaClock;
    bool m_running = true;

    AppState m_appState;
    MenuState m_menuState;

    std::shared_ptr<GameEngine> m_gameEngine;
    sf::View m_view;

    std::vector<std::function<void()>> m_systems;

    void init();

public:
    Editor();

    void run();
    void update();
    void quit();

    void updateStyles();
    void toggleTheme();
    sf::RenderWindow& window();

    void sUserInput();
    void sGUI();
};
