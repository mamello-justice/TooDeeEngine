#pragma once

#include <string>

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
