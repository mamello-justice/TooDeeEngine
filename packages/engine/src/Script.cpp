#include "Script.hpp"

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjs.h>
#endif

#include "Assets.hpp"
#include "Components.hpp"

Script::Script(const std::string& path, const std::string& content) : m_path(path), m_content(content) {}

const std::string& Script::getPath() const {
    return m_path;
}

const std::string& Script::getContent() const {
    return m_content;
}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
void JS_LoadModule(JSContext* ctx, const std::string& scriptName, const Script& script) {
    JSValue moduleEval = JS_Eval(ctx, script.getContent().c_str(), script.getContent().size(), script.getPath().c_str(), JS_EVAL_TYPE_MODULE);

    if (JS_IsException(moduleEval)) {
        JSValue exception = JS_GetException(ctx);
        const char* str = JS_ToCString(ctx, exception);
        std::cerr << "Exception in script " << scriptName << ": " << str << std::endl;
        JS_FreeCString(ctx, str);
        JS_FreeValue(ctx, exception);
    }

    JS_FreeValue(ctx, moduleEval);
}

void JS_EvalScript(JSContext* ctx, const std::string& script) {
    JSValue mainEval = JS_Eval(ctx, script.c_str(), script.size(), "main.js", JS_EVAL_TYPE_MODULE);

    if (JS_IsException(mainEval)) {
        JSValue exception = JS_GetException(ctx);
        const char* str = JS_ToCString(ctx, exception);
        std::cerr << "Exception in script main.js: " << str << std::endl;
        JS_FreeCString(ctx, str);
        JS_FreeValue(ctx, exception);
    }

    JS_FreeValue(ctx, mainEval);
}

void JS_LoadCreateFunction(JSContext* ctx, const CQJSScript& cScript) {
    auto& script = Assets::Instance().getScript(cScript.name);

    JS_LoadModule(ctx, cScript.name, script);

    std::string mainScript =
        std::format("import * as script from '{}';\n", script.getPath()) +
        "globalThis.onCreate = script.onCreate;";

    JS_EvalScript(ctx, mainScript);
}

void JS_LoadUpdateFunction(JSContext* ctx, const CQJSScript& cScript) {
    auto& script = Assets::Instance().getScript(cScript.name);

    JS_LoadModule(ctx, cScript.name, script);

    std::string mainScript =
        std::format("import * as script from '{}';\n", script.getPath()) +
        "globalThis.onUpdate = script.onUpdate;";

    JS_EvalScript(ctx, mainScript);
}

void JS_LoadDestroyFunction(JSContext* ctx, const CQJSScript& cScript) {
    auto& script = Assets::Instance().getScript(cScript.name);

    JS_LoadModule(ctx, cScript.name, script);

    std::string mainScript =
        std::format("import * as script from '{}';\n", script.getPath()) +
        "globalThis.onCreate = script.onCreate;";

    JS_EvalScript(ctx, mainScript);
}
#endif // TOO_DEE_ENGINE_QJS_SCRIPTING
