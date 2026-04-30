#include <iostream>
#include <string>

#include "spdlog/spdlog.h"

#include "Editor.hpp"

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);

    std::string configPath;
    if (argc == 1) {
        configPath = "config.ini";
    }
    else if (argc == 2) {
        configPath = argv[1];
    }
    else {
        spdlog::info("usage:\n\t {} <config-path>", argv[0]);
        std::exit(1);
    }

    Editor editor(configPath);
    editor.run();
    return 0;
}
