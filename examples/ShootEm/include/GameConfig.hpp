#pragma once

#include <string>

struct WindowConfig
{
    size_t Width, Height;
    bool FullScreen;
};

struct PlayerConfig
{
    float ShapeRadius,
        CollisionRadius,
        Speed,
        OutlineThickness;
    int FillRed, FillGreen, FillBlue,
        OutlineRed, OutlineGreen, OutlineBlue,
        Vertices;
};

struct EnemyConfig
{
    float ShapeRadius,
        CollisionRadius,
        SpeedMin, SpeedMax,
        OutlineThickness;
    int OutlineRed, OutlineGreen, OutlineBlue,
        VerticesMin, VerticesMax,
        SmallLifespan,
        SpawnInterval;
};

struct BulletConfig
{
    float ShapeRadius,
        CollisionRadius,
        Speed,
        OutlineThickness;
    int FillRed, FillGreen, FillBlue,
        OutlineRed, OutlineGreen, OutlineBlue,
        Vertices,
        Lifespan;
};

class GameConfig
{
    GameConfig() = default;

public:
    WindowConfig Window;
    PlayerConfig Player;
    EnemyConfig Enemy;
    BulletConfig Bullet;

    static GameConfig &getInstance();

    void loadFromFile(const std::string &path);
};

std::string getConfigPath();
