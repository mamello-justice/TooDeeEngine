#include "Renderer.hpp"

#include <memory>
#include <SFML/Graphics.hpp>

#include "GameEngine.hpp"

void Renderer::render(std::shared_ptr<GameEngine> engine) {
    engine->renderTarget().clear();

    if (engine->m_shouldRender && engine->currentScene()) {
        // TODO: Remove custom scene rendering after everything is in entity
        engine->currentScene()->sRender();

        for (const auto& e : engine->currentScene()->getEntityManager().getEntities()) {
            if (e->has<CAnimation>() && e->has<CTransform>()) {
                auto& cTrans = e->get<CTransform>();
                auto& cAnim = e->get<CAnimation>();

                auto& texture = Assets::Instance().getTexture(cAnim.animation.getTextureName());
                auto sprite = sf::Sprite(texture, cAnim.animation.getSpriteRect());
                sprite.setOrigin(sprite.getGlobalBounds().size / 2.f);
                sprite.setRotation(sf::radians(cTrans.angle));
                sprite.setPosition({ cTrans.pos.x, cTrans.pos.y });
                sprite.setScale({ cTrans.scale.x, cTrans.scale.y });
                engine->renderTarget().draw(sprite);
            }

            if (e->has<CCircle>() && e->has<CTransform>()) {
                auto& cTrans = e->get<CTransform>();
                auto& cCircle = e->get<CCircle>();

                auto circle = sf::CircleShape(cCircle.radius, cCircle.points);
                circle.setFillColor(cCircle.fill);
                circle.setOutlineColor(cCircle.outline);
                circle.setOutlineThickness(cCircle.thickness);
                circle.setOrigin(Vec2f(cCircle.radius, cCircle.radius));
                circle.setPosition(cTrans.pos);
                engine->renderTarget().draw(circle);
            }

            if (e->has<CRectangle>() && e->has<CTransform>()) {
                auto& cTrans = e->get<CTransform>();
                auto& cRect = e->get<CRectangle>();

                auto rect = sf::RectangleShape(cRect.size);
                rect.setFillColor(cRect.fill);
                rect.setOutlineColor(cRect.outline);
                rect.setOutlineThickness(cRect.thickness);
                rect.setOrigin(cRect.size / 2.f);
                rect.setPosition(cTrans.pos);
                engine->renderTarget().draw(rect);
            }
        }
    }

    engine->renderTarget().display();
}
