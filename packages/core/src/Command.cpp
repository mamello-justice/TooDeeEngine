#include "Command.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>

#ifdef _WIN32
#include "Windows.h"
#include "shellapi.h"
#endif

#include "Utils.hpp"

#ifdef _WIN32
void openFile(const std::filesystem::path& path, HWND hwnd)
#else
void openFile(const std::filesystem::path& path)
#endif
{
    auto preferredPath = std::filesystem::canonical(path).string();
#ifdef _WIN32
    // Windows: Use ShellExecute to open with default app
    std::string winPath = replace(replace(preferredPath, " ", "\\ "), "\\", "\\\\");
    std::cout << "[INFO][Command] Running: " << "code.exe " << winPath << std::endl;
    ShellExecuteA(hwnd, "edit", "code.exe", winPath.c_str(), NULL, SW_SHOWNORMAL);
#elif __APPLE__
    // macOS: Use open
    std::string command = "open \"" + preferredPath + "\"";
    std::cout << "[INFO][Command] Running: " << command << std::endl;
    std::system(command.c_str());
#else
    // Linux/Unix: Use xdg-open
    std::string command = "xdg-open \"" + preferredPath + "\"";
    std::cout << "[INFO][Command] Running: " << command << std::endl;
    std::system(command.c_str());
#endif
}
