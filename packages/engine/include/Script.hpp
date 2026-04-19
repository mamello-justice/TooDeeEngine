#pragma once

#include <string>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjs.h>
#endif

#include "Components.hpp"

class Script {
    std::string m_path;
    std::string m_content;

public:
    /**
     * @brief Default constructor
     */
    Script() = default;

    /**
     * @brief Parameterized constructor
     *
     * @param path The file path to the script
     * @param content The content of the script
     */
    Script(const std::string& path, const std::string& content);

    /**
     * @brief Get the file path of the script
     *
     * @return The file path of the script
     */
    const std::string& getPath() const;

    /**
     * @brief Get the content of the script
     *
     * @return The content of the script
     */
    const std::string& getContent() const;
};

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
void JS_LoadCreateFunction(JSContext*, const CQJSScript&);
void JS_LoadUpdateFunction(JSContext*, const CQJSScript&);
void JS_LoadDestroyFunction(JSContext*, const CQJSScript&);
#endif
