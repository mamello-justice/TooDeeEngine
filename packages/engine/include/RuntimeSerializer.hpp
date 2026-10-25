#pragma once

#include <filesystem>
#include <memory>

class GameEngine;

class RuntimeSerializer {
public:
    static void save(const std::shared_ptr<GameEngine>&, const std::filesystem::path&);
    static void load(const std::shared_ptr<GameEngine>&, const std::filesystem::path&);
};
