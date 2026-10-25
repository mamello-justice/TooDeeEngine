#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class GameEngine;

struct ProjectScene {
    std::string id;
    std::string name;
    std::string section;
    std::filesystem::path sourcePath;
};

struct ProjectDefinition {
    std::filesystem::path sourcePath;
    std::vector<std::filesystem::path> assetConfigPaths;
    std::string defaultScene;
    std::string windowTitle = "TooDeeEngine";
    unsigned int windowWidth = 1280;
    unsigned int windowHeight = 720;
    unsigned int frameRateLimit = 60;
    std::string windowMode = "windowed";
    std::string windowStyle = "default";
    std::vector<ProjectScene> scenes;
};

std::filesystem::path resolveProjectEntry(const std::filesystem::path& path);
ProjectDefinition loadProject(const std::filesystem::path& entryPath);
void loadProjectScenes(const ProjectDefinition& project, const std::shared_ptr<GameEngine>& gameEngine);
void initializeProject(const ProjectDefinition& project, const std::shared_ptr<GameEngine>& gameEngine);
