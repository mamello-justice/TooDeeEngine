#include "SFMLTraits.hpp"

#include <SFML/Graphics.hpp>

#include "Vec2.hpp"

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
namespace qjs
{
    JSValue js_traits<sf::RenderTarget>::wrap(JSContext* ctx, const sf::RenderTarget& renderTarget) noexcept {
        JSValue result = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, result, "size", js_traits<Vec2u>::wrap(ctx, renderTarget.getSize()));
        return result;
    }
}; // namespace qjs
#endif // TOO_DEE_ENGINE_QJS_SCRIPTING
