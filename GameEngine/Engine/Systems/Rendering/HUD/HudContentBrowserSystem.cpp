#include "HudContentBrowserSystem.h"
#include "../../../Tools/ImageLoader.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../Core/ServiceLocator.h"
#include "../../EngineContext.h"
#include "../RenderContext.h"
#include "../../ResourceService.h"
#include "../../HardwareContext.h"
#include <uuid.hpp>
#include <fstream>
#include "../../../Tools/MathHelper.h"

SyResult HudContentBrowserSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rs = ServiceLocator::instance()->Get<ResourceService>();

    rs->updateContentBrowser.AddRaw(this, &HudContentBrowserSystem::UpdatePathFileViewsEvent);
    InitImagesSRV();
    return SyResult();
}

SyResult HudContentBrowserSystem::Destroy()
{
    return SyResult();
}

SyResult HudContentBrowserSystem::Run()
{
    
    static std::filesystem::path previousFrameDirectory = "";
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        return SyResult();
    }
    
    


    ImGui::Begin("Content Browser");
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    if (!std::filesystem::exists(curDirectory))
        curDirectory = assetsDirectory;

    if (curDirectory != previousFrameDirectory)
        InitializePathFileViews(curDirectory);

    previousFrameDirectory = curDirectory;

    auto contentAreaAvailable = ImGui::GetContentRegionAvail();
    static float windowLeftSizeX = startLeftWindowWidth;
    static float windowRightSizeX = contentAreaAvailable.x-startLeftWindowWidth;

   
    itemWasHovered = false;
    Splitter(true, 4.0f, &windowLeftSizeX, &windowRightSizeX, 8, 8, contentAreaAvailable.y);
    DrawTreeFolderWindow(windowLeftSizeX);

    
    ImGui::SameLine();
    ImGui::BeginChild("child2");
    ImGui::Text(curDirectory.string().c_str());
    ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
    ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
    float panelWidth = ImGui::GetContentRegionAvail().x;
    auto widgetStartPos = ImVec2(startCursorPos.x, startCursorPos.y);
    widgetStartPos.y += ImGui::GetScrollY();


    

    int columnCount = std::max((int)(panelWidth / cellSize), 1);
    if (ImGui::BeginTable("ContentTable", columnCount))
    {
        ImGuiListClipper clipper;
        clipper.Begin((int)fileViewsVec.size() / columnCount + int(((int)fileViewsVec.size() % columnCount) != 0));

        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();


                int columnsInRow = columnCount;

                if ((int)fileViewsVec.size() - (row + 1) * columnCount < 0)
                    columnsInRow = fileViewsVec.size() % columnCount;

                for (int column = 0; column < columnsInRow; column++)
                {
                    int index = row * columnCount + column;

                    ImGui::TableSetColumnIndex(column);
                    auto directoryEntry = fileViewsVec[index];
                    const auto& path = directoryEntry.path();
                    auto relativePath = std::filesystem::relative(directoryEntry.path(), assetsDirectory);
                    std::string  fileNameStr = relativePath.filename().string();
                    std::string fileExtension = relativePath.extension().string();

                    if (fileExtension == ".meta")
                        continue;

                    ImVec4 bg = bg_col;
                    if (selectedFiles.count(directoryEntry) > 0)
                        bg = bg_col_selected;

                    ImGui::ImageButton(directoryEntry.path().string().data(), iconSRV[fileAssetTypeVec[index]],
                        ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 0), ImVec2(1, 1), bg, tint_col);


                    if (ImGui::BeginDragDropSource())
                    {
                        auto uuidTemp = rs->GetUUIDFromPath(directoryEntry);
                        std::string uuid = boost::lexical_cast<std::string>(uuidTemp);

                        uuid.shrink_to_fit();
                        ImGui::SetDragDropPayload("_CONTENTBROWSER", (uuid.data()), uuid.size());
                        selectedFiles.insert(directoryEntry);
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
                        {

                            for (auto& item : selectedFiles)
                            {
                                ResourceHelper::MoveFile_(item, directoryEntry);

                            }
                            selectedFiles.clear();
                            InitializePathFileViews(curDirectory);
                        }
                        ImGui::EndDragDropTarget();
                    }



                    if (ImGui::IsItemHovered())
                    {
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {

                            if (directoryEntry.is_directory())
                            {
                                curDirectory /= path.filename();
                                selectedFiles.clear();
                            }
                        }
                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        {
                            if (!io.KeyCtrl)
                                selectedFiles.clear();
                            if (selectedFiles.count(directoryEntry) == 0)
                            {
                                auto uuid = rs->GetUUIDFromPath(directoryEntry);
                                ec->hudData.selectedContent = { uuid, rs->resourceLibrary[uuid].assetType };
                                ec->hudData.selectedEntityIDs.clear();
                                selectedFiles.insert(directoryEntry);

                            }
                            else
                                selectedFiles.erase(directoryEntry);


                        }
                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                        {

                            selectedFiles.insert(directoryEntry);
                            ImGui::OpenPopup("ContentPopUp");
                            ProcessPopUp();
                        }

                        itemWasHovered = true;
                    }



                    if (directoryEntry != renamingFileName)
                        ImGui::Text(fileNameStr.c_str());
                    else
                    {

                        if (ImGui::InputText("RenamingText", renamingFileString, IM_ARRAYSIZE(renamingFileString), ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            renamingFileName = (renamingFileName.remove_filename().string() + renamingFileString);
                            if (std::filesystem::exists(renamingFileName))
                            {
                                renamingFileName = directoryEntry;
                            }
                            else
                            {
                                ResourceHelper::RenameFile(directoryEntry, renamingFileName);
                                InitializePathFileViews(curDirectory);
                            }
                            renamingFileName = "";

                        }
                        if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        {
                            renamingFileName = "";
                        }
                    }
                }
            }
        }
        ImGui::EndTable();
    }

    if (!itemWasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !wasOverPopup)
    {
        selectedFiles.clear();
    }

    auto end = ImGui::GetContentRegionMax().y;
    auto scrll = ImGui::GetScrollY();
    auto widgetEndPos = ImVec2(widgetStartPos.x + ImGui::GetContentRegionMax().x, widgetStartPos.y + ImGui::GetContentRegionMax().y+ ImGui::GetScrollY());
    int gap = 20;
    bool isWidgetHovered = io.MousePos.x > widgetStartPos.x && io.MousePos.x < widgetEndPos.x&& io.MousePos.y>widgetStartPos.y-gap && io.MousePos.y < widgetEndPos.y;

    //Popups!

  

    if (io.MouseClicked[1] && isWidgetHovered )
    {
        ImGui::OpenPopup("ContentPopUp");
    }

    ProcessPopUp();
    
    ImGui::EndChild();
    ImGui::End();
    return SyResult();
}

void HudContentBrowserSystem::InitImagesSRV()
{
    int size;
    for (auto& keyVal : cb_iconsPathsDB)
    {
        iconSRV[keyVal.first] = nullptr;
        ImageLoader::LoadTextureFromFile(keyVal.second.string().c_str(), &iconSRV[keyVal.first], &size, &size);
    }
}

void HudContentBrowserSystem::ProcessPopUp()
{


    int selected_Popup = -1;
    enum ESelectedCategory
    {
        None,
        MainPopup,
        CreatePopup

    };
    ESelectedCategory selectedCategory = None;
    const char* mainPopupsNames[] = { "Delete","Rename","Show in explorer"};
    const char* createPopupNames[] = { "Folder","Material" };
    std::vector<std::function<bool(std::set<std::filesystem::path>)>> mainPredicates = {
        [](const std::set<std::filesystem::path>& set) {return set.size()>0; },
        [](const std::set<std::filesystem::path>& set) {return set.size() == 1; },
        [](const std::set<std::filesystem::path>& set) {return true; }
    };
    wasOverPopup = ImGui::BeginPopup("ContentPopUp");
    if (wasOverPopup)
    {
        //wasOverPopup = true;
        //ImGui::SeparatorText("Create");
        if (ImGui::BeginMenu("Create..."))
        {
            for (int i = 0; i < IM_ARRAYSIZE(createPopupNames); i++)
                if (ImGui::MenuItem(createPopupNames[i]))
                {
                    selected_Popup = i;
                    selectedCategory = CreatePopup;
                }
            ImGui::EndMenu();
        }

        for (int i = 0; i < IM_ARRAYSIZE(mainPopupsNames); i++)
            if (ImGui::MenuItem(mainPopupsNames[i],"",false,mainPredicates[i](selectedFiles)))
            {
                selected_Popup = i;
                selectedCategory = MainPopup;
            }
        
        ImGui::EndPopup();
    }

    switch (selectedCategory)
    {
        case CreatePopup:
            switch (selected_Popup)
            {
                case 0:
                    ResourceHelper::ConstructFile(curDirectory.string() + "\\NewFolder");
                    break;
                case 1:
                    auto createdFile = ResourceHelper::ConstructFile(curDirectory.string() + "\\Material.mat");
                    ResourceHelper::CreateMetaFile_(createdFile, EAssetType::ASSET_MATERIAL);
                    ResourceHelper::FillFileWithBaseData(createdFile, EAssetType::ASSET_MATERIAL);
                    rs->resourceLibrary[rs->GetUUIDFromPath(createdFile)] = { EAssetType::ASSET_MATERIAL,createdFile.string() };
                    break;
            }
            InitializePathFileViews(curDirectory);
            break;
        case MainPopup:
            switch (selected_Popup)
            {
                case 0:
                    for (auto& selectedFile : selectedFiles)
                    {
                        
                        CheckRemovingResourceReferences(selectedFile);
                        ResourceHelper::RemoveFile(selectedFile);
                    }
                    rs->updateContentBrowser.Broadcast(true);
                    break;
                case 1:
                    renamingFileName = *selectedFiles.begin();
                    break;
                case 2:
                    std::string resPath = curDirectory.string();
                    std::replace(resPath.begin(), resPath.end(),'/','\\');
                    std::string command = "explorer /select," + resPath ;
                    system(command.c_str());
            }
            break;
    }
}

void HudContentBrowserSystem::DrawTreeFolderWindow(float windowLeftSizeX)
{
    ImGui::BeginChild("child", ImVec2(windowLeftSizeX, ImGui::GetContentRegionAvail().y));
    RenderTree(".\\Game");
    ImGui::EndChild();
}

void HudContentBrowserSystem::RenderTree(std::filesystem::path path)
{
    if (!std::filesystem::exists(path))
        return;

    ImGuiIO& io = ImGui::GetIO(); (void)io;
   

    for (auto& directoryEntry : std::filesystem::directory_iterator(path))
    {
        if (directoryEntry.path().filename().extension() == ".meta")
            continue;

        ImGuiTreeNodeFlags treeFlags= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen;
        if (!directoryEntry.is_directory())
        {
            continue;
        }
        
        bool opened = ImGui::TreeNodeEx(directoryEntry.path().filename().string().c_str(), treeFlags);

        if (ImGui::IsItemHovered())
            itemWasHovered = true;

        if (ImGui::BeginDragDropSource())
        {
            selectedFiles.clear();
            ImGui::SetDragDropPayload("_CONTENTBROWSER", nullptr, 0);
            selectedFiles.insert(directoryEntry);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
            {

                for (auto& item : selectedFiles)
                {
                    ResourceHelper::MoveFile_(item, directoryEntry);
                }
                if (!std::filesystem::exists(curDirectory))
                    curDirectory = assetsDirectory;
                InitializePathFileViews(curDirectory);
                selectedFiles.clear();

            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::IsItemHovered()&&ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            curDirectory = directoryEntry;
            selectedFiles.clear();
        }

        if(opened)
        {
            RenderTree(directoryEntry);
            ImGui::TreePop();
        }

       
    }

    
}

bool HudContentBrowserSystem::Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Min.x += window->DC.CursorPos.x;
    bb.Min.y += window->DC.CursorPos.y;
    bb.Max = CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    bb.Max.x += bb.Min.x;
    bb.Max.y += bb.Min.y;
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

void HudContentBrowserSystem::InitializePathFileViews(const std::filesystem::path& path)
{
    fileViewsVec.clear();
    fileAssetTypeVec.clear();

    for (auto& directoryEntry : std::filesystem::directory_iterator(path))
    {
        const auto& path = directoryEntry.path();
        auto relativePath = std::filesystem::relative(directoryEntry.path(), assetsDirectory);

        if (relativePath.extension() == ".meta")
            continue;
        
        fileViewsVec.push_back(directoryEntry);
        if (!directoryEntry.is_directory())
            fileAssetTypeVec.push_back(rs->resourceLibrary[rs->GetUUIDFromPath(directoryEntry.path())].assetType);
        else
            fileAssetTypeVec.push_back(EAssetType::ASSET_FOLDER);
    }


}

void HudContentBrowserSystem::UpdatePathFileViewsEvent(bool shouldUpdate)
{
    if (shouldUpdate)
        InitializePathFileViews(curDirectory);
}

bool HudContentBrowserSystem::CheckRemovingResourceReferences(const std::filesystem::path& path)
{
   /* auto uuid=rs->GetUUIDFromPath(path);
    if (rs->resourceLibrary[uuid].resource.use_count() > 0)
    {
        ImGui::OpenPopup("RemoveResource");
    }
    return false;*/
    return false;
}
