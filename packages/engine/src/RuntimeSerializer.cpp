#include "RuntimeSerializer.hpp"

#include <array>
#include <charconv>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Assets.hpp"
#include "Components.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "GameEngine.hpp"
#include "INIReader.h"
#include "Scene.hpp"

namespace
{
    struct EntitySnapshot {
        size_t id = 0;
        std::string tag;
        std::vector<std::string> components;
    };

    struct SceneSnapshot {
        std::string name;
        bool paused = true;
        std::map<int, std::string> actions;
        std::map<size_t, EntitySnapshot> entities;
    };

    std::string encodeHex(std::string_view value) {
        static constexpr char digits[] = "0123456789abcdef";
        std::string encoded;
        encoded.reserve(value.size() * 2);
        for (const unsigned char byte : value) {
            encoded.push_back(digits[byte >> 4]);
            encoded.push_back(digits[byte & 0x0f]);
        }
        return encoded;
    }

    int hexValue(char character) {
        if (character >= '0' && character <= '9') return character - '0';
        if (character >= 'a' && character <= 'f') return character - 'a' + 10;
        if (character >= 'A' && character <= 'F') return character - 'A' + 10;
        return -1;
    }

    std::string decodeHex(std::string_view value) {
        if (value.size() % 2 != 0) throw std::runtime_error("Malformed hex value in runtime save");
        std::string decoded;
        decoded.reserve(value.size() / 2);
        for (size_t index = 0; index < value.size(); index += 2) {
            const auto high = hexValue(value[index]);
            const auto low = hexValue(value[index + 1]);
            if (high < 0 || low < 0) throw std::runtime_error("Malformed hex value in runtime save");
            decoded.push_back(static_cast<char>((high << 4) | low));
        }
        return decoded;
    }

    std::string encodeText(const std::string& value) {
        return "hex:" + encodeHex(value);
    }

    std::string decodeText(const std::string& value) {
        if (!value.starts_with("hex:")) return value;
        return decodeHex(std::string_view(value).substr(4));
    }

    std::vector<std::string> splitSection(const std::string& section) {
        std::vector<std::string> parts;
        size_t start = 0;
        while (start <= section.size()) {
            const auto end = section.find('.', start);
            parts.push_back(section.substr(start, end == std::string::npos ? end : end - start));
            if (end == std::string::npos) break;
            start = end + 1;
        }
        return parts;
    }

    std::string scenePrefix(const std::string& name) {
        return "scene." + encodeHex(name);
    }

    std::string readText(const INIReader& reader, const std::string& section, const std::string& key, const std::string& fallback = {}) {
        return decodeText(reader.GetString(section, key, fallback));
    }

    std::string readRequiredText(const INIReader& reader, const std::string& section, const std::string& key) {
        if (!reader.HasValue(section, key)) throw std::runtime_error("Runtime save is missing [" + section + "] " + key);
        return readText(reader, section, key);
    }

    template<typename Number>
    Number readNumber(const INIReader& reader, const std::string& section, const std::string& key, Number fallback) {
        if (!reader.HasValue(section, key)) return fallback;
        const auto text = reader.GetString(section, key, "");
        try {
            if constexpr (std::is_floating_point_v<Number>) {
                size_t parsed = 0;
                const auto number = std::stod(text, &parsed);
                if (parsed != text.size()) throw std::runtime_error("invalid number");
                return static_cast<Number>(number);
            }
            else {
                Number number{};
                const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), number);
                if (error != std::errc{} || end != text.data() + text.size()) throw std::runtime_error("invalid integer");
                return number;
            }
        }
        catch (const std::exception&) {
            throw std::runtime_error("Invalid value for [" + section + "] " + key + ": " + text);
        }
    }

    sf::Color readColor(const INIReader& reader, const std::string& section, const std::string& field) {
        return sf::Color(
            readNumber<uint8_t>(reader, section, field + "_r", 0),
            readNumber<uint8_t>(reader, section, field + "_g", 0),
            readNumber<uint8_t>(reader, section, field + "_b", 0),
            readNumber<uint8_t>(reader, section, field + "_a", 255));
    }

    void writeColor(std::ostream& output, const std::string& field, const sf::Color& color) {
        output << field << "_r=" << static_cast<unsigned int>(color.r) << '\n'
            << field << "_g=" << static_cast<unsigned int>(color.g) << '\n'
            << field << "_b=" << static_cast<unsigned int>(color.b) << '\n'
            << field << "_a=" << static_cast<unsigned int>(color.a) << '\n';
    }

    void writeComponent(std::ostream& output, const std::string& section, const Entity& entity) {
        if (entity.has<CTransform>()) {
            const auto& value = entity.get<CTransform>();
            output << '[' << section << ".component.transform]\n"
                << "x=" << value.pos.x << '\n' << "y=" << value.pos.y << '\n'
                << "previous_x=" << value.prevPos.x << '\n' << "previous_y=" << value.prevPos.y << '\n'
                << "velocity_x=" << value.velocity.x << '\n' << "velocity_y=" << value.velocity.y << '\n'
                << "scale_x=" << value.scale.x << '\n' << "scale_y=" << value.scale.y << '\n'
                << "angle=" << value.angle << "\n\n";
        }
        if (entity.has<CLabel>()) {
            const auto& value = entity.get<CLabel>();
            output << '[' << section << ".component.label]\n"
                << "text=" << encodeText(value.label) << '\n'
                << "font=" << encodeText(value.font) << '\n'
                << "size=" << value.size << "\n\n";
        }
        if (entity.has<CCircle>()) {
            const auto& value = entity.get<CCircle>();
            output << '[' << section << ".component.circle]\n"
                << "radius=" << value.radius << '\n'
                << "points=" << value.points << '\n'
                << "thickness=" << value.thickness << '\n';
            writeColor(output, "fill", value.fill);
            writeColor(output, "outline", value.outline);
            output << '\n';
        }
        if (entity.has<CRectangle>()) {
            const auto& value = entity.get<CRectangle>();
            output << '[' << section << ".component.rectangle]\n"
                << "width=" << value.size.x << '\n' << "height=" << value.size.y << '\n'
                << "thickness=" << value.thickness << '\n';
            writeColor(output, "fill", value.fill);
            writeColor(output, "outline", value.outline);
            output << '\n';
        }
        if (entity.has<CBoundingBox>()) {
            const auto& value = entity.get<CBoundingBox>();
            output << '[' << section << ".component.bounding_box]\n"
                << "width=" << value.size.x << '\n' << "height=" << value.size.y << "\n\n";
        }
        if (entity.has<CBoundingCircle>()) {
            output << '[' << section << ".component.bounding_circle]\n"
                << "radius=" << entity.get<CBoundingCircle>().radius << "\n\n";
        }
        if (entity.has<CGravity>()) {
            output << '[' << section << ".component.gravity]\n"
                << "value=" << entity.get<CGravity>().gravity << "\n\n";
        }
        if (entity.has<CState>()) {
            output << '[' << section << ".component.state]\n"
                << "value=" << encodeText(entity.get<CState>().state) << "\n\n";
        }
        if (entity.has<CScore>()) {
            output << '[' << section << ".component.score]\n"
                << "value=" << entity.get<CScore>().score << "\n\n";
        }
        if (entity.has<CLifespan>()) {
            const auto& value = entity.get<CLifespan>();
            output << '[' << section << ".component.lifespan]\n"
                << "lifespan=" << value.lifespan << '\n'
                << "remaining=" << value.remaining << "\n\n";
        }
        if (entity.has<CInput>()) {
            const auto& value = entity.get<CInput>();
            output << '[' << section << ".component.input]\n"
                << "up=" << value.up << '\n' << "down=" << value.down << '\n'
                << "left=" << value.left << '\n' << "right=" << value.right << '\n'
                << "shoot=" << value.shoot << '\n' << "can_shoot=" << value.canShoot << '\n'
                << "can_jump=" << value.canJump << "\n\n";
        }
        if (entity.has<CAnimation>()) {
            const auto& value = entity.get<CAnimation>();
            output << '[' << section << ".component.animation]\n"
                << "name=" << encodeText(value.animation.getName()) << '\n'
                << "frame=" << value.animation.getCurrentFrame() << '\n'
                << "repeat=" << value.repeat << "\n\n";
        }
#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
        if (entity.has<CQJSScript>()) {
            output << '[' << section << ".component.qjs_script]\n"
                << "name=" << encodeText(entity.get<CQJSScript>().name) << "\n\n";
        }
#endif
#ifdef TOO_DEE_ENGINE_LUA_SCRIPTING
        if (entity.has<CLuaScript>()) {
            output << '[' << section << ".component.lua_script]\n"
                << "name=" << encodeText(entity.get<CLuaScript>().name) << "\n\n";
        }
#endif
    }

    void restoreComponent(const INIReader& reader, const std::string& section, const std::string& type, const std::shared_ptr<Entity>& entity) {
        if (type == "transform") {
            const Vec2f position(static_cast<float>(readNumber<double>(reader, section, "x", 0)), static_cast<float>(readNumber<double>(reader, section, "y", 0)));
            const Vec2f velocity(static_cast<float>(readNumber<double>(reader, section, "velocity_x", 0)), static_cast<float>(readNumber<double>(reader, section, "velocity_y", 0)));
            const Vec2f scale(static_cast<float>(readNumber<double>(reader, section, "scale_x", 1)), static_cast<float>(readNumber<double>(reader, section, "scale_y", 1)));
            auto& transform = entity->add<CTransform>(position, velocity, scale, static_cast<float>(readNumber<double>(reader, section, "angle", 0)));
            transform.prevPos.x = static_cast<float>(readNumber<double>(reader, section, "previous_x", position.x));
            transform.prevPos.y = static_cast<float>(readNumber<double>(reader, section, "previous_y", position.y));
        }
        else if (type == "label") {
            entity->add<CLabel>(readText(reader, section, "text"), readText(reader, section, "font", "default"), readNumber<unsigned int>(reader, section, "size", 30));
        }
        else if (type == "circle") {
            entity->add<CCircle>(readNumber<float>(reader, section, "radius", 10), readNumber<size_t>(reader, section, "points", 50), readColor(reader, section, "fill"), readColor(reader, section, "outline"), readNumber<float>(reader, section, "thickness", 1));
        }
        else if (type == "rectangle") {
            const Vec2f size(static_cast<float>(readNumber<double>(reader, section, "width", 10)), static_cast<float>(readNumber<double>(reader, section, "height", 10)));
            entity->add<CRectangle>(size, readColor(reader, section, "fill"), readColor(reader, section, "outline"), readNumber<float>(reader, section, "thickness", 1));
        }
        else if (type == "bounding_box") {
            const Vec2f size(static_cast<float>(readNumber<double>(reader, section, "width", 0)), static_cast<float>(readNumber<double>(reader, section, "height", 0)));
            entity->add<CBoundingBox>(size);
        }
        else if (type == "bounding_circle") entity->add<CBoundingCircle>(readNumber<float>(reader, section, "radius", 0));
        else if (type == "gravity") entity->add<CGravity>(readNumber<float>(reader, section, "value", 0));
        else if (type == "state") entity->add<CState>(readText(reader, section, "value", "jumping"));
        else if (type == "score") entity->add<CScore>(readNumber<int>(reader, section, "value", 0));
        else if (type == "lifespan") {
            auto& lifespan = entity->add<CLifespan>(readNumber<int>(reader, section, "lifespan", 0));
            lifespan.remaining = readNumber<int>(reader, section, "remaining", lifespan.lifespan);
        }
        else if (type == "input") {
            auto& input = entity->add<CInput>();
            input.up = reader.GetBoolean(section, "up", false);
            input.down = reader.GetBoolean(section, "down", false);
            input.left = reader.GetBoolean(section, "left", false);
            input.right = reader.GetBoolean(section, "right", false);
            input.shoot = reader.GetBoolean(section, "shoot", false);
            input.canShoot = reader.GetBoolean(section, "can_shoot", true);
            input.canJump = reader.GetBoolean(section, "can_jump", true);
        }
        else if (type == "animation") {
            const auto name = readRequiredText(reader, section, "name");
            auto& animation = entity->add<CAnimation>(Assets::Instance().getAnimation(name), reader.GetBoolean(section, "repeat", false));
            animation.animation.setCurrentFrame(readNumber<size_t>(reader, section, "frame", 0));
        }
#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
        else if (type == "qjs_script") entity->add<CQJSScript>(readRequiredText(reader, section, "name"));
#endif
#ifdef TOO_DEE_ENGINE_LUA_SCRIPTING
        else if (type == "lua_script") entity->add<CLuaScript>(readRequiredText(reader, section, "name"));
#endif
        else throw std::runtime_error("Unsupported component in runtime save: " + type);
    }
}

void RuntimeSerializer::save(const std::shared_ptr<GameEngine>& gameEngine, const std::filesystem::path& path) {
    if (!gameEngine) throw std::invalid_argument("Cannot save a null GameEngine");
    if (!gameEngine->m_sceneMap.contains(gameEngine->m_currentScene)) {
        throw std::runtime_error("Cannot save a game without a valid current scene");
    }

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) throw std::runtime_error("Could not open runtime save for writing: " + path.string());
    output << "[save]\nversion=1\ncurrent_scene=" << encodeText(gameEngine->m_currentScene) << '\n'
        << "simulation_speed=" << gameEngine->m_simulationSpeed << '\n'
        << "running=" << gameEngine->m_running << '\n'
        << "should_render=" << gameEngine->m_shouldRender << "\n\n";

    for (const auto& [name, scene] : gameEngine->m_sceneMap) {
        const auto prefix = scenePrefix(name);
        output << '[' << prefix << "]\n"
            << "name=" << encodeText(name) << '\n'
            << "paused=" << scene->m_paused << "\n\n";
        for (const auto& [scan, action] : scene->m_actionMap) {
            output << '[' << prefix << ".action." << static_cast<int>(scan) << "]\n"
                << "name=" << encodeText(action) << "\n\n";
        }
        for (const auto& entity : scene->m_entityManager.getAllEntities()) {
            if (!entity->isAlive()) continue;
            const auto entityPrefix = prefix + ".entity." + std::to_string(entity->id());
            output << '[' << entityPrefix << "]\n"
                << "tag=" << encodeText(entity->tag()) << "\n\n";
            writeComponent(output, entityPrefix, *entity);
        }
    }
    output.flush();
    if (!output) throw std::runtime_error("Failed while writing runtime save: " + path.string());
}

void RuntimeSerializer::load(const std::shared_ptr<GameEngine>& gameEngine, const std::filesystem::path& path) {
    if (!gameEngine) throw std::invalid_argument("Cannot load into a null GameEngine");
    if (!std::filesystem::is_regular_file(path)) throw std::runtime_error("Runtime save does not exist: " + path.string());
    INIReader reader(path.string());
    if (reader.ParseError() != 0) throw std::runtime_error("Could not parse runtime save: " + reader.ParseErrorMessage());
    if (reader.GetInteger("save", "version", 0) != 1) throw std::runtime_error("Unsupported runtime save version");
    const auto currentScene = readRequiredText(reader, "save", "current_scene");
    const auto simulationSpeed = readNumber<size_t>(reader, "save", "simulation_speed", 1);
    if (simulationSpeed == 0) throw std::runtime_error("Simulation speed in runtime save must be positive");
    const auto running = reader.GetBoolean("save", "running", true);
    const auto shouldRender = reader.GetBoolean("save", "should_render", false);

    std::map<std::string, SceneSnapshot> scenes;
    for (const auto& section : reader.Sections()) {
        const auto parts = splitSection(section);
        if (parts.size() == 2 && parts[0] == "scene") {
            const auto key = decodeHex(parts[1]);
            SceneSnapshot snapshot;
            snapshot.name = readRequiredText(reader, section, "name");
            snapshot.paused = reader.GetBoolean(section, "paused", true);
            if (!scenes.emplace(key, std::move(snapshot)).second) throw std::runtime_error("Duplicate scene in runtime save");
        }
    }
    if (scenes.empty()) throw std::runtime_error("Runtime save contains no scenes");

    for (const auto& section : reader.Sections()) {
        const auto parts = splitSection(section);
        if (parts.size() < 2 || parts[0] != "scene") continue;
        const auto sceneKey = decodeHex(parts[1]);
        auto sceneIt = scenes.find(sceneKey);
        if (sceneIt == scenes.end()) throw std::runtime_error("Runtime save section references an unknown scene");
        auto& scene = sceneIt->second;
        if (parts.size() == 4 && parts[2] == "action") {
            int scan = 0;
            const auto& scanText = parts[3];
            const auto [end, error] = std::from_chars(scanText.data(), scanText.data() + scanText.size(), scan);
            if (error != std::errc{} || end != scanText.data() + scanText.size() || scan < -1 || scan > static_cast<int>(sf::Keyboard::Scan::Pause)) {
                throw std::runtime_error("Invalid keyboard scancode in runtime save");
            }
            scene.actions[scan] = readRequiredText(reader, section, "name");
        }
        else if (parts.size() == 4 && parts[2] == "entity") {
            size_t id = 0;
            const auto& idText = parts[3];
            const auto [end, error] = std::from_chars(idText.data(), idText.data() + idText.size(), id);
            if (error != std::errc{} || end != idText.data() + idText.size() || id == std::numeric_limits<size_t>::max()) {
                throw std::runtime_error("Invalid entity ID in runtime save");
            }
            EntitySnapshot entity;
            entity.id = id;
            entity.tag = readRequiredText(reader, section, "tag");
            if (!scene.entities.emplace(id, std::move(entity)).second) throw std::runtime_error("Duplicate entity ID in runtime save");
        }
        else if (parts.size() == 6 && parts[2] == "entity" && parts[4] == "component") {
            size_t id = 0;
            const auto& idText = parts[3];
            const auto [end, error] = std::from_chars(idText.data(), idText.data() + idText.size(), id);
            if (error != std::errc{} || end != idText.data() + idText.size()) throw std::runtime_error("Invalid entity ID in runtime save");
            const auto entity = scene.entities.find(id);
            if (entity == scene.entities.end()) throw std::runtime_error("Component references an unknown entity in runtime save");
            entity->second.components.push_back(section);
        }
    }

    std::map<std::string, std::shared_ptr<Scene>> restoredScenes;
    std::map<std::string, std::shared_ptr<Scene>> reconstructedScenes;
    for (const auto& [key, snapshot] : scenes) {
        if (snapshot.name.empty() || reconstructedScenes.contains(snapshot.name)) throw std::runtime_error("Invalid or duplicate scene name in runtime save");
        auto scene = std::make_shared<Scene>(gameEngine);
        scene->setPaused(snapshot.paused);
        for (const auto& [scan, action] : snapshot.actions) {
            scene->registerAction(static_cast<sf::Keyboard::Scan>(scan), action);
        }
        for (const auto& [id, entitySnapshot] : snapshot.entities) {
            auto entity = scene->getEntityManager().addEntity(entitySnapshot.tag, id);
            for (const auto& component : entitySnapshot.components) {
                const auto parts = splitSection(component);
                restoreComponent(reader, component, parts.back(), entity);
            }
        }
        scene->getEntityManager().update();
        reconstructedScenes.emplace(snapshot.name, std::move(scene));
    }

    if (!reconstructedScenes.contains(currentScene)) throw std::runtime_error("Current scene is missing from runtime save: " + currentScene);
    for (auto& [name, reconstructed] : reconstructedScenes) {
        const auto existing = gameEngine->m_sceneMap.find(name);
        if (existing == gameEngine->m_sceneMap.end()) {
            restoredScenes.emplace(name, std::move(reconstructed));
            continue;
        }

        auto& target = existing->second;
        target->m_entityManager = std::move(reconstructed->m_entityManager);
        target->m_actionMap = std::move(reconstructed->m_actionMap);
        target->m_paused = reconstructed->m_paused;
        target->m_gameEngine = gameEngine;
        restoredScenes.emplace(name, target);
    }
    gameEngine->m_sceneMap = std::move(restoredScenes);
    gameEngine->m_currentScene = currentScene;
    gameEngine->m_simulationSpeed = simulationSpeed;
    gameEngine->m_running = running;
    gameEngine->m_shouldRender = shouldRender;
}
