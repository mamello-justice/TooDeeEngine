#pragma once

#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "imgui.h"

#include "GameEngine.hpp"
#include "Vec2.hpp"

struct AppState {
    bool DarkTheme = true;
    bool DrawGrid = false;
    bool DrawTextures = true;
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

class Editor {
    bool                        m_running = true;

    AppState                    m_appState;
    MenuState                   m_menuState;

    sf::Clock                   m_deltaClock;
    std::shared_ptr<GameEngine> m_gameEngine;
    ImVec2                      m_viewportSize;

    std::vector<std::function<void()>> m_preSystems;
    std::vector<std::function<void()>> m_postSystems;

    std::string m_consoleText;

    std::shared_ptr<Entity> m_selectedEntity;

    const Vec2f m_gridSize = { 64, 64 };

    void init();

public:
    Editor();

    void run();
    void update();
    void quit();

    void play();
    void pause();
    void stop();

    void updateStyles();
    void toggleTheme();
    void toggleGrid();
    void toggleTextures();
    void toggleCollisions();
    void toggleAnimationNames();
    sf::RenderWindow& window();

    void sViewport();
    void sUserInput();
    void sRender();
    void sGUI();
};
