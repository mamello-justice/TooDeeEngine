#include "Editor.hpp"

#include <iostream>

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "Styles.hpp"

Editor::Editor() { init(); }

void Editor::init() {
    m_window.create(sf::VideoMode::getDesktopMode(), "TooDeeEditor", sf::State::Fullscreen);

    if (!ImGui::SFML::Init(m_window)) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
    }
    ImGui::SetupImGuiStyle(true, 1);

    m_systems.push_back(std::bind(&Editor::sUserInput, this));
    m_systems.push_back(std::bind(&Editor::sGUI, this));
}

void Editor::run() {
    while (m_running) { update(); }
}

void Editor::update() {
    ImGui::SFML::Update(m_window, m_deltaClock.restart());

    m_window.clear();

    for (auto system : m_systems) { system(); }

    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Editor::quit() {
    m_running = false;
}

void Editor::sUserInput() {
    while (auto event = m_window.pollEvent()) {
        ImGui::SFML::ProcessEvent(m_window, *event);

        if (event->is<sf::Event::Closed>()) { quit(); }

        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (
                keyPressed->scancode == sf::Keyboard::Scancode::F4 &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl))) {
                quit();
            }
            else if (
                keyPressed->scancode == sf::Keyboard::Scancode::Q &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl)) &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RShift))) {
                quit();
            }
        }

    }
}

void Editor::sGUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")) { /* Handle New action */ }
            if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Handle Open action */ }
            if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) { quit(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Example")) { /* Handle New action */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Example")) { /* Handle New action */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Example")) { /* Handle New action */ }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
