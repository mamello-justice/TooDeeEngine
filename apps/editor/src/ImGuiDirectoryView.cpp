#include "ImGuiDirectoryView.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "imgui.h"

#include "TooDeeCore.hpp"

void recursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator) {
    for (const std::filesystem::directory_entry& entry : directoryIterator)
    {
        DirectoryNode& childNode = parentNode.Children.emplace_back();
        childNode.FullPath = entry.path().string();
        childNode.FileName = entry.path().filename().string();
        if (childNode.IsDirectory = entry.is_directory(); childNode.IsDirectory)
            recursivelyAddDirectoryNodes(childNode, std::filesystem::directory_iterator(entry));
    }

    auto moveDirectoriesToFront = [](const DirectoryNode& a, const DirectoryNode& b) { return (a.IsDirectory > b.IsDirectory); };
    std::sort(parentNode.Children.begin(), parentNode.Children.end(), moveDirectoriesToFront);
}

DirectoryNode createDirectoryNodeTreeFromPath(const std::filesystem::path& rootPath) {
    DirectoryNode rootNode;
    rootNode.FullPath = rootPath.string();
    rootNode.FileName = rootPath.filename().string();
    if (rootNode.IsDirectory = std::filesystem::is_directory(rootPath); rootNode.IsDirectory)
        recursivelyAddDirectoryNodes(rootNode, std::filesystem::directory_iterator(rootPath));
    return rootNode;
}

DirectoryNode createDirectoryNodeTreeFromMap(const std::map<std::string, std::string>& paths) {
    auto base = std::filesystem::current_path();

    DirectoryNode rootNode;
    rootNode.FullPath = base.string();
    rootNode.AbsolutePath = ".";
    rootNode.FileName = "Root";

    for (const auto& [_key, value] : paths) {
        auto fullSystemPath = base / value;
        auto relativePath = std::filesystem::relative(fullSystemPath, base);

        DirectoryNode* currentNode = &rootNode;
        for (const auto& part : relativePath) {
            auto fullSystemPartPath = base / currentNode->AbsolutePath / part;
            auto relativePartPath = std::filesystem::relative(fullSystemPartPath, base);

            auto it = std::find_if(currentNode->Children.begin(), currentNode->Children.end(),
                [&part](const DirectoryNode& node) { return node.FileName == part.string(); });

            if (it == currentNode->Children.end()) {
                currentNode->Children.push_back({
                    fullSystemPartPath.string(),
                    relativePartPath.string(),
                    part.string(),
                    {},
                    std::filesystem::is_directory(fullSystemPartPath)
                    });
                currentNode = &currentNode->Children.back();
            }
            else {
                currentNode = &(*it);
            }
        }
    }

    return rootNode;
}

void ImGuiDirectoryNode(const DirectoryNode& parentNode) {
    ImGui::PushID(&parentNode);
    if (parentNode.IsDirectory)
    {
        if (ImGui::TreeNodeEx(parentNode.FileName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth))
        {
            for (const DirectoryNode& childNode : parentNode.Children)
                ImGuiDirectoryNode(childNode);
            ImGui::TreePop();
        }
    }
    else
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth;
        if (ImGui::TreeNodeEx(parentNode.FileName.c_str(), flags)) {}
        if (ImGui::IsItemClicked()) {
            openFile(parentNode.FullPath);
        }
    }
    ImGui::PopID();
}
