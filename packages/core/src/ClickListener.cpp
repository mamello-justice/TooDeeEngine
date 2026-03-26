#include "ClickListener.hpp"

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Vec2.hpp"

namespace Listener
{
    void ClickListener::registerListener(
        const std::shared_ptr<sf::Shape>& shape,
        const sf::Mouse::Button& button,
        const std::shared_ptr<ClickCallback>& func) {
        m_listeners[button].push_back(std::pair(shape, func));
    }

    void ClickListener::onClick(
        const Vec2f& target,
        const sf::Mouse::Button& button) {
        for (auto& handler : m_listeners[button]) {
            if (handler.first->getGlobalBounds().contains(target)) {
                (*handler.second)();
            }
        }
    }

} // namespace Listener
