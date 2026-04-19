#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

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

