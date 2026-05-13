#pragma once

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjspp.h>

namespace qjs
{
    /** Conversion traits for RenderTarget
     */
    template<>
    struct js_traits<sf::RenderTarget> {
        static sf::RenderTarget unwrap(JSContext* ctx, JSValueConst val) = delete;
        static JSValue wrap(JSContext* ctx, const sf::RenderTarget&) noexcept;
    };

}; // namespace qjs
#endif // TOO_DEE_ENGINE_QJS_SCRIPTING
