#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "TooDeeEngine.hpp"

namespace ImGui
{
    const float ICON_SIZE = 20.f;
    const sf::Vector2f ICON_SIZE_2(ICON_SIZE, ICON_SIZE);

    bool IconButton(const std::string& id, const std::string& iconName, bool enabled = true) {
        if (!enabled) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        bool clicked = false;
        try {
            auto& texture = Assets::Instance().getTexture(iconName);
            auto sprite = sf::Sprite(texture);
            clicked = ImageButton(id.c_str(), sprite, ICON_SIZE_2);
        }
        catch (std::runtime_error _e) {
            clicked = Button(id.c_str());
        }
        if (!enabled) ImGui::PopStyleVar();
        return enabled && clicked;
    }
}
