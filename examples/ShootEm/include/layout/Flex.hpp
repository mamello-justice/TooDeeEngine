#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

#include "Core.hpp"

namespace Layout
{

    class Flex : public sf::Shape
    {
        Vec2f m_size;
        float m_padTop = 0.f,
              m_padRight = 0.f,
              m_padBottom = 0.f,
              m_padLeft = 0.f;
        std::vector<std::shared_ptr<sf::Shape>> m_children;
        std::shared_ptr<sf::RectangleShape> m_bg;
        sf::Color m_bgColor;
        sf::Color m_olColor;
        float m_gap = 0;

        void updateBackground();

        void updateChildren();

        void updateSize();

    public:
        Flex();

        Flex(const sf::Color &bgColor, const sf::Color &olColor);

        void addChild(std::shared_ptr<sf::Shape> child);

        void setBackgroundColor(const sf::Color &bgColor);

        void setGap(float gap);

        void setOutlineColor(const sf::Color &bgColor);

        Vec2f getSize() const;

        void setSize(Vec2f size);

        void setPadding(float pad);

        void setPadding(float padY, float padX);

        void setPadding(float padTop, float padRight, float padBottom, float padLeft);

        void setPosition_(Vec2f pos);

        std::size_t getPointCount() const;

        sf::Vector2f getPoint(std::size_t index) const;

        sf::Vector2f getGeometricCenter() const;

        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    };

} // namespace Layout
