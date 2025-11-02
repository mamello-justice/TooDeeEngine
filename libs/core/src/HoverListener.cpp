#include "HoverListener.hpp"

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Vec2.hpp"

namespace Listener
{
    void HoverListener::registerListener(
        const std::shared_ptr<sf::Shape>& shape,
        const std::shared_ptr<HoverCallback>& func) {
        m_listeners.push_back(std::make_tuple(shape, func, false));
    }

    void HoverListener::onHover(const Vec2f& target) {
        for (auto& [shape, func, hovered] : m_listeners) {
            auto pos = shape->getGlobalBounds().position;
            auto size = shape->getGlobalBounds().size;
            auto hovered = shape->getGlobalBounds().contains(target);
            if (shape->getGlobalBounds().contains(target)) {
                if (!hovered) {
                    (*func)(true);
                    hovered = true;
                }
            }
            else if (hovered) {
                (*func)(false);
                hovered = false;
            }
        }
    }

} // namespace Listener
