#include "Project.hpp"

#include <algorithm>
#include <cctype>
#include <limits>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <SFML/Graphics.hpp>

#include "INIReader.h"
#include "TooDeeEngine.hpp"

namespace
{
    std::filesystem::path absoluteNormalized(const std::filesystem::path& path) {
        return std::filesystem::absolute(path).lexically_normal();
    }

    std::string lowercase(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
            });
        return value;
    }

    std::unique_ptr<INIReader> readIni(const std::filesystem::path& path) {
        if (!std::filesystem::is_regular_file(path)) {
            throw std::runtime_error("Project file does not exist: " + path.string());
        }

        auto reader = std::make_unique<INIReader>(path.string());
        if (reader->ParseError() != 0) {
            throw std::runtime_error("Could not parse " + path.string() + ": " + reader->ParseErrorMessage());
        }
        return reader;
    }

    std::filesystem::path resolveRelative(const std::filesystem::path& owner, const std::string& value) {
        if (value.empty()) {
            throw std::runtime_error("A referenced file path cannot be empty in " + owner.string());
        }
        auto path = std::filesystem::path(value);
        return absoluteNormalized(path.is_absolute() ? path : owner.parent_path() / path);
    }

    unsigned int readUnsigned(const INIReader& reader, const std::string& section, const std::string& key, unsigned int fallback) {
        if (!reader.HasValue(section, key)) {
            return fallback;
        }

        const auto value = reader.GetString(section, key, "");
        try {
            size_t parsed = 0;
            const auto number = std::stoul(value, &parsed);
            if (parsed != value.size() || number == 0 || number > std::numeric_limits<unsigned int>::max()) {
                throw std::runtime_error("invalid value");
            }
            return static_cast<unsigned int>(number);
        }
        catch (const std::exception&) {
            throw std::runtime_error("Invalid positive integer for [" + section + "] " + key + ": " + value);
        }
    }

    std::string sceneIdFromSection(const std::string& section) {
        constexpr std::string_view prefix = "scene.";
        if (!section.starts_with(prefix)) {
            return {};
        }

        auto id = section.substr(prefix.size());
        if (id.empty() || id.find('.') != std::string::npos) {
            return {};
        }
        return id;
    }

    std::string directChildId(const std::string& section, const std::string& parentPath) {
        const auto fullPrefix = parentPath + ".";
        if (!section.starts_with(fullPrefix)) return {};
        const auto id = section.substr(fullPrefix.size());
        if (id.empty() || id.find('.') != std::string::npos) return {};
        return id;
    }

    std::pair<std::string, std::string> componentFromSection(const std::string& section, const std::string& sceneId) {
        const auto prefix = "scene." + sceneId + ".entity.";
        if (!section.starts_with(prefix)) return {};
        const auto remainder = section.substr(prefix.size());
        const auto entityEnd = remainder.find('.');
        if (entityEnd == std::string::npos) return {};
        const auto entityId = remainder.substr(0, entityEnd);
        constexpr std::string_view componentMarker = ".component.";
        if (entityId.empty() || !remainder.substr(entityEnd).starts_with(componentMarker)) return {};
        const auto type = remainder.substr(entityEnd + componentMarker.size());
        if (type.empty() || type.find('.') != std::string::npos) return {};
        return { entityId, type };
    }

    Vec2f readVec2(const INIReader& reader, const std::string& section, const std::string& xKey, const std::string& yKey, Vec2f fallback = {}) {
        return {
            static_cast<float>(reader.GetReal(section, xKey, fallback.x)),
            static_cast<float>(reader.GetReal(section, yKey, fallback.y))
        };
    }

    sf::Color readColor(const INIReader& reader, const std::string& section, const std::string& key, const sf::Color& fallback) {
        if (!reader.HasValue(section, key)) return fallback;
        const auto value = reader.GetString(section, key, "");
        unsigned int channels[4] = { 0, 0, 0, 255 };
        size_t start = 0;
        for (size_t index = 0; index < 4; ++index) {
            const auto end = value.find(',', start);
            const auto token = value.substr(start, end == std::string::npos ? end : end - start);
            try {
                size_t parsed = 0;
                const auto channel = std::stoul(token, &parsed);
                if (parsed != token.size() || channel > 255) throw std::runtime_error("invalid color");
                channels[index] = static_cast<unsigned int>(channel);
            }
            catch (const std::exception&) {
                throw std::runtime_error("Invalid RGBA color for [" + section + "] " + key + ": " + value);
            }
            if (end == std::string::npos) {
                if (index < 2) throw std::runtime_error("Color requires at least RGB channels for [" + section + "] " + key);
                break;
            }
            start = end + 1;
        }
        return sf::Color(channels[0], channels[1], channels[2], channels[3]);
    }

    sf::Keyboard::Scan readScancode(const std::string& value) {
        const auto key = lowercase(value);
        if (key.size() == 1 && key[0] >= 'a' && key[0] <= 'z') {
            constexpr sf::Keyboard::Scan letters[] = {
                sf::Keyboard::Scan::A, sf::Keyboard::Scan::B, sf::Keyboard::Scan::C, sf::Keyboard::Scan::D,
                sf::Keyboard::Scan::E, sf::Keyboard::Scan::F, sf::Keyboard::Scan::G, sf::Keyboard::Scan::H,
                sf::Keyboard::Scan::I, sf::Keyboard::Scan::J, sf::Keyboard::Scan::K, sf::Keyboard::Scan::L,
                sf::Keyboard::Scan::M, sf::Keyboard::Scan::N, sf::Keyboard::Scan::O, sf::Keyboard::Scan::P,
                sf::Keyboard::Scan::Q, sf::Keyboard::Scan::R, sf::Keyboard::Scan::S, sf::Keyboard::Scan::T,
                sf::Keyboard::Scan::U, sf::Keyboard::Scan::V, sf::Keyboard::Scan::W, sf::Keyboard::Scan::X,
                sf::Keyboard::Scan::Y, sf::Keyboard::Scan::Z
            };
            return letters[key[0] - 'a'];
        }
        if (key.size() == 1 && key[0] >= '0' && key[0] <= '9') {
            constexpr sf::Keyboard::Scan digits[] = {
                sf::Keyboard::Scan::Num0, sf::Keyboard::Scan::Num1, sf::Keyboard::Scan::Num2,
                sf::Keyboard::Scan::Num3, sf::Keyboard::Scan::Num4, sf::Keyboard::Scan::Num5,
                sf::Keyboard::Scan::Num6, sf::Keyboard::Scan::Num7, sf::Keyboard::Scan::Num8,
                sf::Keyboard::Scan::Num9
            };
            return digits[key[0] - '0'];
        }
        static const std::unordered_map<std::string, sf::Keyboard::Scan> namedKeys = {
            { "space", sf::Keyboard::Scan::Space },{ "enter", sf::Keyboard::Scan::Enter },
            { "escape", sf::Keyboard::Scan::Escape },{ "tab", sf::Keyboard::Scan::Tab },
            { "left", sf::Keyboard::Scan::Left },{ "right", sf::Keyboard::Scan::Right },
            { "up", sf::Keyboard::Scan::Up },{ "down", sf::Keyboard::Scan::Down },
            { "lshift", sf::Keyboard::Scan::LShift },{ "rshift", sf::Keyboard::Scan::RShift },
            { "lctrl", sf::Keyboard::Scan::LControl },{ "rctrl", sf::Keyboard::Scan::RControl }
        };
        if (const auto it = namedKeys.find(key); it != namedKeys.end()) return it->second;
        throw std::runtime_error("Unsupported keyboard scancode: " + value);
    }

    std::shared_ptr<Scene> loadScene(const ProjectScene& definition, const std::shared_ptr<GameEngine>& gameEngine) {
        auto document = readIni(definition.sourcePath);
        const auto& sceneSection = definition.section;
        if (!document->HasSection(sceneSection)) {
            throw std::runtime_error("Scene document must define [" + sceneSection + "]: " + definition.sourcePath.string());
        }

        auto scene = std::make_shared<Scene>(gameEngine);
        scene->setPaused(document->GetBoolean(sceneSection, "paused", false));
        std::unordered_map<std::string, std::shared_ptr<Entity>> entities;

        for (const auto& section : document->Sections()) {
            const auto entityId = directChildId(section, "scene." + definition.id + ".entity");
            if (entityId.empty()) continue;
            const auto tag = document->GetString(section, "tag", "default");
            entities.emplace(entityId, scene->getEntityManager().addEntity(tag));
        }

        for (const auto& section : document->Sections()) {
            const auto actionId = directChildId(section, "scene." + definition.id + ".action");
            if (!actionId.empty()) {
                const auto action = document->GetString(section, "name", "");
                if (action.empty()) throw std::runtime_error("Action section must provide name: [" + section + "]");
                scene->registerAction(readScancode(actionId), action);
                continue;
            }

            const auto [entityId, componentType] = componentFromSection(section, definition.id);
            if (entityId.empty()) continue;
            const auto type = lowercase(componentType);
            const auto entityIt = entities.find(entityId);
            if (entityIt == entities.end()) {
                throw std::runtime_error("Component references an unknown entity in [" + section + "]: " + entityId);
            }
            const auto& entity = entityIt->second;
            if (type == "transform") {
                const auto pos = readVec2(*document, section, "x", "y");
                const auto velocity = readVec2(*document, section, "velocity_x", "velocity_y");
                const auto scale = readVec2(*document, section, "scale_x", "scale_y", { 1.f, 1.f });
                const auto angle = static_cast<float>(document->GetReal(section, "angle", 0));
                entity->add<CTransform>(pos, velocity, scale, angle);
            }
            else if (type == "label") {
                entity->add<CLabel>(document->GetString(section, "text", ""), document->GetString(section, "font", "default"), readUnsigned(*document, section, "size", 30));
            }
            else if (type == "circle") {
                const auto fill = readColor(*document, section, "fill", sf::Color::Black);
                const auto outline = readColor(*document, section, "outline", sf::Color::White);
                const auto points = readUnsigned(*document, section, "points", 50);
                const auto thickness = static_cast<float>(document->GetReal(section, "thickness", 1));
                const auto radius = static_cast<float>(document->GetReal(section, "radius", 10));
                entity->add<CCircle>(radius, points, fill, outline, thickness);
            }
            else if (type == "rectangle") {
                const Vec2f size = {
                    static_cast<float>(document->GetReal(section, "width", 10)),
                    static_cast<float>(document->GetReal(section, "height", 10))
                };
                const auto fill = readColor(*document, section, "fill", sf::Color::Black);
                const auto outline = readColor(*document, section, "outline", sf::Color::White);
                const auto thickness = static_cast<float>(document->GetReal(section, "thickness", 1));
                entity->add<CRectangle>(size, fill, outline, thickness);
            }
            else if (type == "bounding_box") {
                const Vec2f size = {
                    static_cast<float>(document->GetReal(section, "width", 0)),
                    static_cast<float>(document->GetReal(section, "height", 0))
                };
                entity->add<CBoundingBox>(size);
            }
            else if (type == "bounding_circle") {
                entity->add<CBoundingCircle>(static_cast<float>(document->GetReal(section, "radius", 0)));
            }
            else if (type == "gravity") {
                entity->add<CGravity>(static_cast<float>(document->GetReal(section, "value", 0)));
            }
            else if (type == "state") {
                entity->add<CState>(document->GetString(section, "value", "jumping"));
            }
            else if (type == "score") {
                entity->add<CScore>(static_cast<int>(document->GetInteger(section, "value", 0)));
            }
            else if (type == "lifespan") {
                entity->add<CLifespan>(static_cast<int>(document->GetInteger(section, "value", 0)));
            }
            else if (type == "input") {
                auto& input = entity->add<CInput>();
                input.up = document->GetBoolean(section, "up", false);
                input.down = document->GetBoolean(section, "down", false);
                input.left = document->GetBoolean(section, "left", false);
                input.right = document->GetBoolean(section, "right", false);
                input.shoot = document->GetBoolean(section, "shoot", false);
                input.canShoot = document->GetBoolean(section, "can_shoot", true);
                input.canJump = document->GetBoolean(section, "can_jump", true);
            }
            else if (type == "animation") {
                const auto name = document->GetString(section, "name", "");
                if (name.empty()) throw std::runtime_error("Animation component must provide name: [" + section + "]");
                entity->add<CAnimation>(Assets::Instance().getAnimation(name), document->GetBoolean(section, "repeat", false));
            }
            else {
                throw std::runtime_error("Unsupported component type in [" + section + "]: " + type);
            }
        }

        for (const auto& section : document->Sections()) {
            const auto actionPrefix = "scene." + definition.id + ".action.";
            if (!section.starts_with(actionPrefix)) continue;
            if (directChildId(section, "scene." + definition.id + ".action").empty()) {
                throw std::runtime_error("Invalid action section: [" + section + "]");
            }
        }

        gameEngine->changeScene(definition.id, scene);
        return scene;
    }
}

std::filesystem::path resolveProjectEntry(const std::filesystem::path& path) {
    const auto absolutePath = absoluteNormalized(path);
    if (std::filesystem::is_directory(absolutePath)) {
        const auto iniPath = absolutePath / "main.ini";
        const auto tdePath = absolutePath / "main.tde";
        const bool hasIni = std::filesystem::is_regular_file(iniPath);
        const bool hasTde = std::filesystem::is_regular_file(tdePath);
        if (hasIni && hasTde) {
            throw std::runtime_error("Project directory contains both main.ini and main.tde: " + absolutePath.string());
        }
        if (hasIni) return iniPath;
        if (hasTde) return tdePath;
        if (std::filesystem::is_regular_file(absolutePath / "main.lua") || std::filesystem::is_regular_file(absolutePath / "main.js")) {
            throw std::runtime_error("Lua and JavaScript project entrypoints are not supported yet; use main.ini or main.tde");
        }
        throw std::runtime_error("Project directory must contain main.ini or main.tde: " + absolutePath.string());
    }

    const auto extension = lowercase(absolutePath.extension().string());
    if (extension == ".lua" || extension == ".js") {
        throw std::runtime_error("Lua and JavaScript project entrypoints are not supported yet; use main.ini or main.tde");
    }
    if (extension != ".ini" && extension != ".tde") {
        throw std::runtime_error("Project entrypoint must have an .ini or .tde extension: " + absolutePath.string());
    }
    if (!std::filesystem::is_regular_file(absolutePath)) {
        throw std::runtime_error("Project entrypoint does not exist: " + absolutePath.string());
    }
    return absolutePath;
}

ProjectDefinition loadProject(const std::filesystem::path& entryPath) {
    ProjectDefinition project;
    project.sourcePath = resolveProjectEntry(entryPath);
    auto manifest = readIni(project.sourcePath);

    if (!manifest->HasValue("project", "version")) {
        throw std::runtime_error("Project manifest must specify [project] version=1");
    }
    if (manifest->GetInteger("project", "version", 0) != 1) {
        throw std::runtime_error("Unsupported project schema version in " + project.sourcePath.string());
    }

    project.assetConfigPaths.push_back(project.sourcePath);
    const auto assetFile = manifest->GetString("asset", "file", "");
    const auto legacyAssetConfig = manifest->GetString("project", "config", "");
    std::vector<std::pair<std::string, std::string>> configFiles;
    for (const auto& section : manifest->Sections()) {
        if (!section.starts_with("config.")) continue;
        const auto name = section.substr(std::string_view("config.").size());
        if (name.empty()) {
            throw std::runtime_error("Config section name cannot be empty");
        }
        const auto file = manifest->GetString(section, "file", "");
        if (file.empty()) {
            throw std::runtime_error("Config section must specify file: [" + section + "]");
        }
        configFiles.emplace_back(name, file);
    }
    if (!assetFile.empty() && !legacyAssetConfig.empty()) {
        throw std::runtime_error("Specify combined assets with [asset] file or legacy [project] config, not both");
    }
    if ((!assetFile.empty() || !legacyAssetConfig.empty()) && !configFiles.empty()) {
        throw std::runtime_error("Use either [config.<name>] sections or a legacy combined asset config, not both");
    }

    const auto combinedAssetFile = !assetFile.empty() ? assetFile : legacyAssetConfig;
    if (!combinedAssetFile.empty()) {
        const auto assetPath = resolveRelative(project.sourcePath, combinedAssetFile);
        if (!std::filesystem::is_regular_file(assetPath)) {
            throw std::runtime_error("Asset config file does not exist: " + assetPath.string());
        }
        project.assetConfigPaths.push_back(assetPath);
    }
    else {
        for (const auto& [name, file] : configFiles) {
            const auto assetPath = resolveRelative(project.sourcePath, file);
            if (!std::filesystem::is_regular_file(assetPath)) {
                throw std::runtime_error("Config file does not exist for [" + name + "]: " + assetPath.string());
            }
            project.assetConfigPaths.push_back(assetPath);
        }
    }

    std::unique_ptr<INIReader> externalWindow;
    const INIReader* window = manifest.get();
    const auto windowFile = manifest->GetString("window", "file", "");
    if (!windowFile.empty()) {
        auto windowPath = resolveRelative(project.sourcePath, windowFile);
        externalWindow = readIni(windowPath);
        if (!externalWindow->HasSection("window")) {
            throw std::runtime_error("Window config must define [window]: " + windowPath.string());
        }
        window = externalWindow.get();
    }

    project.windowTitle = window->GetString("window", "title", project.windowTitle);
    project.windowWidth = readUnsigned(*window, "window", "width", project.windowWidth);
    project.windowHeight = readUnsigned(*window, "window", "height", project.windowHeight);
    project.frameRateLimit = readUnsigned(*window, "window", "frame_rate", project.frameRateLimit);
    project.windowMode = lowercase(window->GetString("window", "mode", project.windowMode));
    project.windowStyle = lowercase(window->GetString("window", "style", project.windowStyle));
    if (project.windowMode != "windowed" && project.windowMode != "fullscreen") {
        throw std::runtime_error("[window] mode must be windowed or fullscreen");
    }
    if (project.windowStyle != "default" && project.windowStyle != "none" && project.windowStyle != "titlebar" && project.windowStyle != "close" && project.windowStyle != "resize") {
        throw std::runtime_error("[window] style must be default, none, titlebar, close, or resize");
    }

    for (const auto& section : manifest->Sections()) {
        const auto id = sceneIdFromSection(section);
        if (id.empty()) continue;

        ProjectScene scene;
        scene.id = id;
        scene.section = section;
        scene.sourcePath = project.sourcePath;
        const auto sceneFile = manifest->GetString(section, "file", "");
        if (!sceneFile.empty()) {
            scene.sourcePath = resolveRelative(project.sourcePath, sceneFile);
            auto sceneDocument = readIni(scene.sourcePath);
            if (!sceneDocument->HasSection(section)) {
                throw std::runtime_error("Scene file must define [" + section + "]: " + scene.sourcePath.string());
            }
            scene.name = sceneDocument->GetString(section, "name", id);
        }
        else {
            scene.name = manifest->GetString(section, "name", id);
        }
        project.scenes.push_back(std::move(scene));
    }

    if (project.scenes.empty()) {
        throw std::runtime_error("Project manifest must define at least one [scene.<id>] section");
    }

    project.defaultScene = manifest->GetString("project", "default_scene", project.scenes.front().id);
    const auto defaultScene = std::find_if(project.scenes.begin(), project.scenes.end(), [&project](const ProjectScene& scene) {
        return scene.id == project.defaultScene;
        });
    if (defaultScene == project.scenes.end()) {
        throw std::runtime_error("Default scene is not registered: " + project.defaultScene);
    }

    return project;
}

void initializeProject(const ProjectDefinition& project, const std::shared_ptr<GameEngine>& gameEngine) {
    auto& assets = Assets::Instance();
    for (const auto& configPath : project.assetConfigPaths) {
        assets.loadFromFile(configPath.filename(), configPath.parent_path());
    }

    const auto size = sf::Vector2u(project.windowWidth, project.windowHeight);
    const auto state = project.windowMode == "fullscreen" ? sf::State::Fullscreen : sf::State::Windowed;
    std::uint32_t style = sf::Style::Default;
    if (project.windowStyle == "none") style = 0;
    else if (project.windowStyle == "titlebar") style = sf::Style::Titlebar;
    else if (project.windowStyle == "close") style = sf::Style::Titlebar | sf::Style::Close;
    else if (project.windowStyle == "resize") style = sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
    gameEngine->window().create(sf::VideoMode(size), project.windowTitle, style, state);
    gameEngine->window().setFramerateLimit(project.frameRateLimit);
    gameEngine->m_shouldRender = true;
    if (!gameEngine->renderTarget().resize(gameEngine->window().getSize())) {
        throw std::runtime_error("Could not resize the render target to the project window size");
    }

    loadProjectScenes(project, gameEngine);
}

void loadProjectScenes(const ProjectDefinition& project, const std::shared_ptr<GameEngine>& gameEngine) {
    std::shared_ptr<Scene> defaultScene;
    for (const auto& scene : project.scenes) {
        auto loadedScene = loadScene(scene, gameEngine);
        if (scene.id == project.defaultScene) defaultScene = std::move(loadedScene);
    }
    if (!defaultScene) throw std::runtime_error("Default scene was not loaded: " + project.defaultScene);
    gameEngine->changeScene(project.defaultScene, defaultScene);
}
