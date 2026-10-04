#include "ImGuiDirectoryView.hpp"

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"
#include "imgui-SFML.h"

#include "TooDeeCore.hpp"
#include "Assets.hpp"

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

namespace
{
    bool matchesAssetType(const std::filesystem::path& path, int typeFilter) {
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char character) { return static_cast<char>(std::tolower(character)); });

        switch (typeFilter) {
        case 1:
            return extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
                extension == ".bmp" || extension == ".tga";
        case 2:
            return extension == ".ttf" || extension == ".otf";
        case 3:
            return extension == ".js" || extension == ".lua";
        case 4:
            return extension != ".png" && extension != ".jpg" && extension != ".jpeg" &&
                extension != ".bmp" && extension != ".tga" && extension != ".ttf" &&
                extension != ".otf" && extension != ".js" && extension != ".lua";
        default:
            return true;
        }
    }

    bool matchesSearch(const std::string& value, std::string_view search) {
        if (search.empty()) {
            return true;
        }

        std::string loweredValue = value;
        std::string loweredSearch(search);
        auto lowercase = [](unsigned char character) { return static_cast<char>(std::tolower(character)); };
        std::transform(loweredValue.begin(), loweredValue.end(), loweredValue.begin(), lowercase);
        std::transform(loweredSearch.begin(), loweredSearch.end(), loweredSearch.begin(), lowercase);
        return loweredValue.find(loweredSearch) != std::string::npos;
    }

    bool hasVisibleChildren(
        const DirectoryNode& node,
        std::string_view search,
        int typeFilter) {
        for (const auto& child : node.Children) {
            if (child.IsDirectory) {
                if (hasVisibleChildren(child, search, typeFilter)) {
                    return true;
                }
            }
            else if (matchesSearch(child.FileName, search) && matchesAssetType(child.FullPath, typeFilter)) {
                return true;
            }
        }
        return false;
    }
}

bool ImGuiAssetDirectoryNode(
    const DirectoryNode& node,
    std::string_view search,
    int typeFilter,
    const std::optional<ImVec2>& dropPosition,
    std::filesystem::path& dropTarget) {
    const bool visible = node.IsDirectory
        ? hasVisibleChildren(node, search, typeFilter)
        : matchesSearch(node.FileName, search) && matchesAssetType(node.FullPath, typeFilter);
    if (!visible) {
        return false;
    }

    ImGui::PushID(&node);
    std::string iconName = "file_icon";
    if (node.IsDirectory) {
        iconName = "folder_icon";
    }
    else {
        std::string extension = std::filesystem::path(node.FileName).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
            extension == ".bmp" || extension == ".tga") {
            iconName = "file_image_icon";
        }
        else if (extension == ".js") {
            iconName = "file_js_icon";
        }
        else if (extension == ".lua") {
            iconName = "file_code_icon";
        }
        else if (extension == ".ini") {
            iconName = "file_ini_icon";
        }
        else if (extension == ".txt") {
            iconName = "file_txt_icon";
        }
    }

    const ImGuiTreeNodeFlags flags = node.IsDirectory
        ? (node.FileName == "assets" ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None)
        : ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;
    const ImGuiID nodeId = ImGui::GetID("##AssetTreeNode");
    const bool open = ImGui::TreeNodeEx("##AssetTreeNode", flags);
    const bool treeNodeClicked = ImGui::IsItemClicked();
    const ImVec2 rowStart = ImGui::GetItemRectMin();
    const ImVec2 rowEnd(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x,
        ImGui::GetItemRectMax().y);

    ImGui::SameLine(0.0f, 0.0f);
    const float iconSize = ImGui::GetTextLineHeight();
    ImGui::Image(
        Assets::Instance().getTexture(iconName),
        sf::Vector2f(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::TextUnformatted(node.FileName.c_str());
    const bool labelClicked = ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    if (node.IsDirectory) {
        if (dropPosition &&
            dropPosition->x >= rowStart.x && dropPosition->x <= rowEnd.x &&
            dropPosition->y >= rowStart.y && dropPosition->y <= rowEnd.y) {
            dropTarget = node.FullPath;
        }

        if (labelClicked) {
            ImGui::GetStateStorage()->SetInt(nodeId, open ? 0 : 1);
        }

        if (open) {
            for (const auto& child : node.Children) {
                ImGuiAssetDirectoryNode(child, search, typeFilter, dropPosition, dropTarget);
            }
            ImGui::TreePop();
        }
    }
    else if (treeNodeClicked || labelClicked) {
        openFile(node.FullPath);
    }

    ImGui::PopID();
    return true;
}
