#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

#include "Vec2.hpp"

namespace Listener
{
    using HoverCallback = std::function<void(bool)>;
    using HoverHandler = std::tuple<
        std::shared_ptr<sf::Shape>,
        std::shared_ptr<HoverCallback>,
        bool>;

    class HoverListener
    {
        std::vector<HoverHandler> m_listeners;

    public:
        HoverListener() = default;

        void registerListener(
            const std::shared_ptr<sf::Shape> &shape,
            const std::shared_ptr<HoverCallback> &func);

        void onHover(const Vec2f &target);
    };

    using Hover = HoverListener;
} // namespace Listener
