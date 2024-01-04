#include "HudContentBrowserSystem.h"
#include "../../../Core/Tools/ImageLoader.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../Core/ServiceLocator.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/RenderContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../Resources/ResourceService.h"
#include <uuid.hpp>
#include <fstream>
#include "../../../Core/Tools/MathHelper.h"
#include <ctime>

SyResult HudContentBrowserSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rs = ServiceLocator::instance()->Get<ResourceService>();

    rs->updateContentBrowser.AddRaw(this, &HudContentBrowserSystem::UpdatePathFileViewsEvent);
    rs->updateFolderHierarchy.AddRaw(this, &HudContentBrowserSystem::UpdateFolderHierarchyEvent);

    InitializeDirectoryTree(".\\Game", dirTree);

    InitImagesSRV();
    return SyResult();
}

SyResult HudContentBrowserSystem::Destroy()
{
    return SyResult();
}

SyResult HudContentBrowserSystem::Run()
{
    OPTICK_EVENT();
    OPTICK_TAG("START","");
    std::clock_t start;
    double duration;
    start = std::clock();
    
    static std::filesystem::path previousFrameDirectory = "";
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        return SyResult();
    }
    
    //if (ImGui::IsKeyPressed(ImGuiKey_T))
    //{
    //    rs->SaveStringToFile("./Game/Assets/hello.scene", "YOPTA");
    //}


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
    OPTICK_EVENT();

    
    ImGui::SameLine();
    ImGui::BeginChild("child2");
    ImGui::Text(curDirectory.string().c_str());
    ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
    ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
    float panelWidth = ImGui::GetContentRegionAvail().x;
    auto widgetStartPos = ImVec2(startCursorPos.x, startCursorPos.y);
    widgetStartPos.y += ImGui::GetScrollY();

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_C))
    {
        ec->hudData.copyBufferAssets.clear();
        ec->hudData.copyBufferAssets = ec->hudData.selectedAssets;
    }

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V))
    {
        for (auto& uuid : ec->hudData.copyBufferAssets)
        {
            std::filesystem::path sourceFile = rs->FindFilePathByUUID(uuid);
            auto targetDirectory = curDirectory.string()+ "\\" + sourceFile.filename().string();

            auto file=ResourceHelper::PastFile(sourceFile, targetDirectory);
        	ResourceHelper::CreateMetaFile_(file, rs->resourceLibrary[uuid].assetType);
        }
        rs->LoadResourceLibrary(curDirectory);
        InitializePathFileViews(curDirectory);
    }
    

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
                    const auto& path = directoryEntry.fileView.path();
                    auto relativePath = directoryEntry.relativePath;
                    std::string  fileNameStr = relativePath.filename().string();
                    std::string fileExtension = relativePath.extension().string();

                    if (fileExtension == ".meta")
                       continue;

                    ImVec4 bg = bg_col;
                    if (selectedFiles.count(directoryEntry.fileView) > 0)
                        bg = bg_col_selected;

                    ImGui::ImageButton(directoryEntry.fileView.path().string().data(), iconSRV[fileAssetTypeVec[index]],
                        ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 0), ImVec2(1, 1), bg, tint_col);


                    if (ImGui::BeginDragDropSource())
                    {
                        auto uuidTemp = rs->GetUUIDFromPath(directoryEntry.fileView);
                        std::string uuid = boost::lexical_cast<std::string>(uuidTemp);

                        uuid.shrink_to_fit();
                        ImGui::SetDragDropPayload("_CONTENTBROWSER", (uuid.data()), uuid.size());
                        selectedFiles.insert(directoryEntry.fileView);
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
                        {

                            for (auto& item : selectedFiles)
                            {
                                ResourceHelper::MoveFile_(item, directoryEntry.fileView);

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

                            if (directoryEntry.fileView.is_directory())
                            {
                                curDirectory /= path.filename();
                                ec->hudData.selectedAssets.clear();
                                selectedFiles.clear();
                            }
                        }
                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        {
                            if (!io.KeyCtrl)
                            {
                                selectedFiles.clear();
                                ec->hudData.selectedAssets.clear();
                            }
                            if (selectedFiles.count(directoryEntry.fileView) == 0)
                            {
                                auto uuid = rs->GetUUIDFromPath(directoryEntry.fileView);
                                ec->hudData.selectedContent = { uuid, rs->resourceLibrary[uuid].assetType };
                                ec->hudData.selectedEntityIDs.clear();
                                ec->hudData.selectedAssets.insert(uuid);
                                selectedFiles.insert(directoryEntry.fileView);

                            }
                            else
                            {
                                selectedFiles.erase(directoryEntry.fileView);
                                ec->hudData.selectedAssets.erase(rs->GetUUIDFromPath(directoryEntry.fileView));
                            }


                        }
                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                        {

                            selectedFiles.insert(directoryEntry.fileView);
                            ec->hudData.selectedAssets.insert(rs->GetUUIDFromPath(directoryEntry.fileView));
                            ImGui::OpenPopup("ContentPopUp");
                            ProcessPopUp();
                        }

                        itemWasHovered = true;
                    }



                    if (directoryEntry.fileView != renamingFileName)
                        ImGui::Text(fileNameStr.c_str());
                    else
                    {

                        if (ImGui::InputText("RenamingText", renamingFileString, IM_ARRAYSIZE(renamingFileString), ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            renamingFileName = (renamingFileName.remove_filename().string() + renamingFileString);
                            if (std::filesystem::exists(renamingFileName))
                            {
                                renamingFileName = directoryEntry.fileView;
                            }
                            else
                            {
                                ResourceHelper::RenameFile(directoryEntry.fileView, renamingFileName);
                                InitializePathFileViews(curDirectory);
                                UpdateFolderHierarchyEvent(true);
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
        ec->hudData.selectedAssets.clear();
    }

    auto end = ImGui::GetContentRegionMax().y;
    auto scrll = ImGui::GetScrollY();
    auto widgetEndPos = ImVec2(widgetStartPos.x + ImGui::GetContentRegionMax().x, widgetStartPos.y + ImGui::GetContentRegionMax().y+ ImGui::GetScrollY());
    int gap = 20;
    bool isWidgetHovered = io.MousePos.x > widgetStartPos.x && io.MousePos.x < widgetEndPos.x&& io.MousePos.y>widgetStartPos.y-gap && io.MousePos.y < widgetEndPos.y;

    //Popups!
    for (auto& item : ec->hudData.selectedAssets)
    {
        std::cout << item << std::endl;
    }
  

    if (io.MouseClicked[1] && isWidgetHovered )
    {
        ImGui::OpenPopup("ContentPopUp");
    }

    ProcessPopUp();
    
    ImGui::EndChild();
    ImGui::End();

    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

    //std::cout << "printf: " << duration << '\n';

    OPTICK_TAG("FINISH", "");
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
    if (ImGui::IsKeyDown(ImGuiKey_F2))
    {
        if (selectedFiles.size()>0)
			renamingFileName = *selectedFiles.begin();
    }


    int selected_Popup = -1;
    enum ESelectedCategory
    {
        None,
        MainPopup,
        CreatePopup
    };
    ESelectedCategory selectedCategory = None;
    const char* mainPopupsNames[] = { "Copy","Paste","Delete","Rename","Show in explorer"};
    const char* createPopupNames[] = { "Folder","Material","Skybox"};
    std::vector<std::function<bool(std::set<std::filesystem::path>)>> mainPredicates = {
        [](const std::set<std::filesystem::path>& set) {return set.size() > 0; },
        [](const std::set<std::filesystem::path>& set) {return true; },
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
                    UpdateFolderHierarchyEvent(true);
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
                    ec->hudData.copyBufferAssets.clear();
                    ec->hudData.copyBufferAssets = ec->hudData.selectedAssets;
                    break;
				case 1:
                    for (auto& uuid : ec->hudData.copyBufferAssets)
                    {
                        std::filesystem::path sourceFile = rs->FindFilePathByUUID(uuid);
                        auto targetDirectory = curDirectory.string() + "\\" + sourceFile.filename().string();

                        auto file = ResourceHelper::PastFile(sourceFile, targetDirectory);
                        ResourceHelper::CreateMetaFile_(file, rs->resourceLibrary[uuid].assetType);
                    }
                    rs->LoadResourceLibrary(curDirectory);
                    InitializePathFileViews(curDirectory);
                    break;

                case 2:
                    for (auto& selectedFile : selectedFiles)
                    {
                        
                        CheckRemovingResourceReferences(selectedFile);
                        ResourceHelper::RemoveFile(selectedFile);
                    }
                    rs->updateContentBrowser.Broadcast(true);
                    break;
                case 3:
                    renamingFileName = *selectedFiles.begin();
                    break;
                case 4:
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
    
   
    RenderTree(dirTree);
    ImGui::EndChild();
}

void HudContentBrowserSystem::RenderTree(DirectoryTree& dirTree)
{
    if (!std::filesystem::exists(dirTree.path))
        return;

    ImGuiIO& io = ImGui::GetIO(); (void)io;
   
    
    for (auto& subDir : dirTree.subDirs)
    {

        ImGuiTreeNodeFlags treeFlags= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen;
        
        
        bool opened = ImGui::TreeNodeEx(subDir.path.filename().string().c_str(), treeFlags);

        if (ImGui::IsItemHovered())
            itemWasHovered = true;

        if (ImGui::BeginDragDropSource())
        {
            selectedFiles.clear();
            ImGui::SetDragDropPayload("_CONTENTBROWSER", nullptr, 0);
            selectedFiles.insert(subDir.path);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
            {

                for (auto& item : selectedFiles)
                {
                    ResourceHelper::MoveFile_(item, subDir.path);
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
            curDirectory = subDir.path;
            selectedFiles.clear();
        }

        if(opened)
        {
            RenderTree(subDir);
            ImGui::TreePop();
        }

       
    }

    
}

void HudContentBrowserSystem::InitializeDirectoryTree(std::filesystem::path path, DirectoryTree& dirTree)
{
    if (!std::filesystem::exists(path))
        return;

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    dirTree.path = path;
    for (auto& directoryEntry : std::filesystem::directory_iterator(path))
    {

        if (!directoryEntry.is_directory())
        {
            continue;
        }

        if (directoryEntry.path().filename().extension() == ".meta")
            continue;

        dirTree.subDirs.push_back(DirectoryTree());
        InitializeDirectoryTree(directoryEntry.path(), dirTree.subDirs[dirTree.subDirs.size()-1]);
        
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
        
        fileViewsVec.push_back(FileViews(directoryEntry, std::filesystem::relative(directoryEntry.path(), assetsDirectory)));
        if (!directoryEntry.is_directory())
            fileAssetTypeVec.push_back(rs->resourceLibrary[rs->GetUUIDFromPath(directoryEntry.path())].assetType);
        else
            fileAssetTypeVec.push_back(EAssetType::ASSET_FOLDER);
    }


}

void HudContentBrowserSystem::UpdatePathFileViewsEvent(bool shouldUpdate)
{
    if (shouldUpdate)
    {
        InitializePathFileViews(curDirectory);
        UpdateFolderHierarchyEvent(true);
    }
}

void HudContentBrowserSystem::UpdateFolderHierarchyEvent(bool shouldUpdate)
{
    dirTree.path = "";
    dirTree.subDirs.clear();
    InitializeDirectoryTree(".\\Game", dirTree);
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
