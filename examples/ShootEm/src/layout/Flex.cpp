#include "Flex.hpp"

#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

namespace Layout
{
    Flex::Flex()
        : Flex(sf::Color::Transparent, sf::Color::Transparent) {}

    Flex::Flex(
        const sf::Color& bgColor,
        const sf::Color& olColor)
        :
        m_bg(std::make_shared<sf::RectangleShape>()),
        m_bgColor(bgColor),
        m_olColor(olColor) {
        updateBackground();
        m_bg->setFillColor(bgColor);
        m_bg->setOutlineColor(olColor);
    }

    void Flex::updateBackground() {
        Vec2f size = { m_size.x + m_padLeft + m_padRight, m_size.y + m_padTop + m_padBottom };
        m_bg->setSize(size);
        m_bg->setOrigin(size / 2.f);
        m_bg->setPosition(getPosition());
    }

    void Flex::updateChildren() {
        float offsetY = -1.f * (getSize().y / 2.f);
        for (auto child : m_children) {
            child->setPosition({ getPosition().x, getPosition().y + offsetY });
            offsetY += child->getGlobalBounds().size.y + m_gap;
        }
    }

    void Flex::updateSize() {
        float width = 0.f, height = 0.f;

        for (auto& child : m_children) {
            auto size = child->getGlobalBounds().size;
            if (size.x > width) {
                width = size.x;
            }
            height += size.y;
            height += m_gap;
        }
        // Remove last item gap
        if (m_children.size() > 1) {
            height -= m_gap;
        }
        setSize({ width, height });
    }

    void Flex::setGap(float gap) {
        m_gap = gap;

        updateSize();
        updateChildren();
    }

    void Flex::setSize(Vec2f size) {
        m_size = size;

        setOrigin(size / 2.f);

        update();
        updateBackground();
    }

    void Flex::setPadding(float pad) {
        setPadding(pad, pad, pad, pad);
    }

    void Flex::setPadding(float padY, float padX) {
        setPadding(padY, padX, padY, padX);
    }

    void Flex::setPadding(float padTop, float padRight, float padBottom, float padLeft) {
        m_padTop = padTop;
        m_padRight = padRight;
        m_padBottom = padBottom;
        m_padLeft = padLeft;
    }

    void Flex::setPosition_(Vec2f pos) {
        setPosition(pos);

        update();
        updateBackground();
        updateChildren();
    }

    void Flex::addChild(std::shared_ptr<sf::Shape> child) {
        m_children.push_back(child);
        updateSize();
        updateChildren();
    }

    void Flex::setBackgroundColor(const sf::Color& bgColor) {
        m_bgColor = bgColor;
        m_bg->setFillColor(bgColor);
    }

    void Flex::setOutlineColor(const sf::Color& olColor) {
        m_olColor = olColor;
        m_bg->setOutlineColor(olColor);
    }

    Vec2f Flex::getSize() const {
        return m_size;
    }

    std::size_t Flex::getPointCount() const {
        return m_bg->getPointCount();
    }

    sf::Vector2f Flex::getPoint(std::size_t index) const {
        return m_bg->getPoint(index);
    }

    sf::Vector2f Flex::getGeometricCenter() const {
        return m_bg->getGeometricCenter();
    }

    void Flex::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(*m_bg, states);

        for (auto child : m_children) {
            target.draw(*child, states);
        }
    }
} //namespace Layout
