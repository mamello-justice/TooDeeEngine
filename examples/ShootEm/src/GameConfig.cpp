#include "GameConfig.hpp"

#include <stdexcept>
#include <string>

#include "Assets.hpp"
#include "INIReader.h"

GameConfig& GameConfig::getInstance() {
    static GameConfig instance;
    return instance;
}

void GameConfig::loadFromFile(const std::string& filename) {
    INIReader reader(filename);

    if (reader.ParseError() != 0) {
        throw std::runtime_error("Can't load config file: " + reader.ParseErrorMessage());
    }

    // Window
    Window.Width = reader.GetUnsigned("settings", "window_width", 1080);
    Window.Height = reader.GetUnsigned("settings", "window_height", 720);
    Window.FullScreen = reader.GetBoolean("settings", "fullscreen", false);

    // Player
    Player.ShapeRadius = static_cast<float>(reader.GetReal("player", "shape_radius", 32.0));
    Player.CollisionRadius = static_cast<float>(reader.GetReal("player", "collision_radius", 32.0));
    Player.Speed = static_cast<float>(reader.GetReal("player", "speed", 5.0));
    Player.Vertices = reader.GetInteger("player", "vertices", 8);

    Player.FillRed = reader.GetInteger("player.fill", "red", 5);
    Player.FillGreen = reader.GetInteger("player.fill", "green", 5);
    Player.FillBlue = reader.GetInteger("player.fill", "blue", 5);

    Player.OutlineRed = reader.GetInteger("player.outline", "red", 255);
    Player.OutlineGreen = reader.GetInteger("player.outline", "green", 0);
    Player.OutlineBlue = reader.GetInteger("player.outline", "blue", 0);
    Player.OutlineThickness = static_cast<float>(reader.GetReal("player.outline", "thickness", 4.0));

    // Enemy
    Enemy.ShapeRadius = static_cast<float>(reader.GetReal("enemy", "shape_radius", 32.0));
    Enemy.CollisionRadius = static_cast<float>(reader.GetReal("enemy", "collision_radius", 32.0));
    Enemy.SmallLifespan = reader.GetInteger("enemy", "small_lifespan", 0);
    Enemy.SpawnInterval = reader.GetInteger("enemy", "spawn_interval", 0);

    Enemy.SpeedMin = static_cast<float>(reader.GetReal("enemy.speed", "min", 3.0));
    Enemy.SpeedMax = static_cast<float>(reader.GetReal("enemy.speed", "max", 3.0));

    Enemy.OutlineRed = reader.GetInteger("enemy.outline", "red", 255);
    Enemy.OutlineGreen = reader.GetInteger("enemy.outline", "green", 255);
    Enemy.OutlineBlue = reader.GetInteger("enemy.outline", "blue", 255);
    Enemy.OutlineThickness = static_cast<float>(reader.GetReal("enemy.outline", "thickness", 2.0));

    Enemy.VerticesMin = reader.GetInteger("enemy.vertices", "min", 3);
    Enemy.VerticesMax = reader.GetInteger("enemy.vertices", "max", 8);

    // Bullet
    Bullet.ShapeRadius = static_cast<float>(reader.GetReal("bullet", "shape_radius", 10.0));
    Bullet.CollisionRadius = static_cast<float>(reader.GetReal("bullet", "collision_radius", 10.0));
    Bullet.Speed = static_cast<float>(reader.GetReal("bullet", "speed", 8.0));
    Bullet.Vertices = reader.GetInteger("bullet", "vertices", 20);
    Bullet.Lifespan = reader.GetInteger("bullet", "lifespan", 90);

    Bullet.FillRed = reader.GetInteger("bullet.fill", "red", 255);
    Bullet.FillGreen = reader.GetInteger("bullet.fill", "green", 255);
    Bullet.FillBlue = reader.GetInteger("bullet.fill", "blue", 255);

    Bullet.OutlineRed = reader.GetInteger("bullet.outline", "red", 255);
    Bullet.OutlineGreen = reader.GetInteger("bullet.outline", "green", 255);
    Bullet.OutlineBlue = reader.GetInteger("bullet.outline", "blue", 255);
    Bullet.OutlineThickness = static_cast<float>(reader.GetReal("bullet.outline", "thickness", 2.0));

    // Load Assets
    Assets::Instance().loadFromFile(filename);
}

#if defined(_DEBUG)
std::string getConfigPath() {
    return "config.ini";
}
#elif defined(_WIN32)
#include <ShlObj.h>
#include <Windows.h>
std::string getConfigPath() {
    PWSTR documentsPath = nullptr;
    std::string configPath;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &documentsPath))) {
        std::wstring wpath = std::wstring(documentsPath) + L"\\ShootEm\\config.ini";

        // Convert wide string to regular string using Windows API
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), (int)wpath.size(), nullptr, 0, nullptr, nullptr);
        configPath.resize(size_needed);
        WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), (int)wpath.size(), &configPath[0], size_needed, nullptr, nullptr);

        CoTaskMemFree(documentsPath);
    }
    else {
        // Fallback to local directory if we can't get Documents folder
        configPath = "config.ini";
    }

    return configPath;
}
#else
std::string getConfigPath() {
    return "config.ini";
}
#endif
