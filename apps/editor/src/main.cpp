#include <iostream>
#include <string>

#include "Editor.hpp"

int main(int argc, char* argv[]) {
    std::string configPath;
    if (argc == 1) {
        configPath = "config.ini";
    }
    else if (argc == 2) {
        configPath = argv[1];
    }
    else {
        std::cout << "usage:\n\t" << argv[0] << " <config-path>" << std::endl;
        std::exit(1);
    }

    Editor editor(configPath);
    editor.run();
    return 0;
}
