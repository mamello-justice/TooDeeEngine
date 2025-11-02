#pragma once

#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

struct MenuState
{
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
    sf::RenderWindow m_window;
    sf::Clock m_deltaClock;
    bool m_running = true;
    std::vector<std::function<void()>> m_systems;

    MenuState m_menuState;

    void init();

public:
    Editor();

    void run();
    void update();
    void quit();

    void sUserInput();
    void sGUI();
};
