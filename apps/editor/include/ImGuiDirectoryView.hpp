#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"

struct DirectoryNode
{
    std::string FullPath;
    std::string AbsolutePath;
    std::string FileName;
    std::vector<DirectoryNode> Children;
    bool IsDirectory;
};

void recursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator);
DirectoryNode createDirectoryNodeTreeFromPath(const std::filesystem::path& rootPath);
DirectoryNode createDirectoryNodeTreeFromMap(const std::map<std::string, std::string>& paths);
void ImGuiDirectoryNode(const DirectoryNode& parentNode);
bool ImGuiAssetDirectoryNode(
    const DirectoryNode& node,
    std::string_view search,
    int typeFilter,
    const std::optional<ImVec2>& dropPosition,
    std::filesystem::path& dropTarget);
