#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "Core.hpp"

namespace Components
{

    class Button : public sf::Shape
    {
        Vec2f m_size;
        std::shared_ptr<sf::Text> m_text;
        std::shared_ptr<sf::RectangleShape> m_bg;
        sf::Color m_fgColor;
        sf::Color m_bgColor;
        sf::Color m_olColor;

        void updateBackground();

    public:
        Button(
            std::shared_ptr<sf::Text> text);

        Button(
            std::shared_ptr<sf::Text> text,
            const sf::Color &fgColor,
            const sf::Color &bgColor,
            const sf::Color &olColor);

        void setSize(Vec2f size);

        void setText(std::string);

        void setTextColor(const sf::Color &fgColor);

        void setBackgroundColor(const sf::Color &bgColor);

        void setOutlineColor(const sf::Color &bgColor);

        Vec2f getSize() const;

        std::size_t getPointCount() const;

        sf::Vector2f getPoint(std::size_t index) const;

        sf::Vector2f getGeometricCenter() const;

        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    };

} // namespace Component
