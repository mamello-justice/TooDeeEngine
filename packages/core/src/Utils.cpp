#include "Utils.hpp"

#include <string>

std::string replace(
    const std::string& path,
    const std::string& pattern,
    const std::string& replacement
) {
    auto pathCopy(path);

    size_t pos = pathCopy.find(pattern);
    while (pos != std::string::npos) {
        pathCopy.replace(pos, pattern.size(), replacement);
        pos = pathCopy.find(pattern, pos + replacement.size());
    }

    return pathCopy;
}
