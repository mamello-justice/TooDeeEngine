#pragma once

#include <functional>
#include <map>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Vec2.hpp"

namespace Listener
{
    using ClickCallback = std::function<void()>;
    using ClickHandler = std::pair<std::shared_ptr<sf::Shape>, std::shared_ptr<ClickCallback>>;

    class ClickListener
    {
        std::map<sf::Mouse::Button, std::vector<ClickHandler>> m_listeners;

    public:
        ClickListener() = default;

        void registerListener(
            const std::shared_ptr<sf::Shape> &shape,
            const sf::Mouse::Button &button,
            const std::shared_ptr<ClickCallback> &func);

        void onClick(const Vec2f &target, const sf::Mouse::Button &button);
    };

    using Click = ClickListener;
} // namespace Listener
