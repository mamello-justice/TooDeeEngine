#include "Script.hpp"

Script::Script(const std::string& path, const std::string& content) : m_path(path), m_content(content) {}

const std::string& Script::getPath() const {
    return m_path;
}

const std::string& Script::getContent() const {
    return m_content;
}
