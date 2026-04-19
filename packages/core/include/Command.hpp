#pragma once

#include <filesystem>

#ifdef _WIN32
#include "Windows.h"
#endif

#ifdef _WIN32
void openFile(const std::filesystem::path& path, HWND hwnd = NULL);
#else
void openFile(const std::filesystem::path& path);
#endif
