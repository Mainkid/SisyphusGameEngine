#include "HudContentBrowserSystem.h"
#include "../../../Tools/ImageLoader.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../Core/ServiceLocator.h"
#include "../../EngineContext.h"
#include "../RenderContext.h"
#include "../../HardwareContext.h"
#include "ResourceHelper.h"
#include <fstream>

void HudContentBrowserSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    InitImagesSRV();
}

void HudContentBrowserSystem::Destroy()
{
    
}


void HudContentBrowserSystem::Run()
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        return;
    }


    ImGui::Begin("Content Browser");
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    auto content = ImGui::GetContentRegionAvail();
   
    if (windowLeftSizeX==0)
        windowLeftSizeX = 300;
    if (windowRightSizeX==0)
        windowRightSizeX = content.x-300;

   
    itemWasHovered = false;
    
    Splitter(true, 4.0f, &windowLeftSizeX, &windowRightSizeX, 8, 8, content.y);
   
    DrawTreeFolderWindow();

    
    ImGui::SameLine();

    ImGui::BeginChild("child2");

    ImGui::Text(curDirectory.string().c_str());

    ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
    ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
    float panelWidth = ImGui::GetContentRegionAvail().x;
    auto widgetStartPos = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
    widgetStartPos.y += ImGui::GetScrollY();

    

    int columnCount = std::max((int)(panelWidth / cellSize), 1);
    ImGui::Columns(columnCount, 0, false);

    /*if (curDirectory != std::filesystem::path(assetsDirectory))
    {
        ImGui::ImageButton("...", iconSRV[EContent::Folder],{ thumbnailSize,thumbnailSize });

       

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
            {

                for (auto& item : selectedFiles)
                {
                    ResourceHelper::MoveFile_(item, curDirectory.parent_path());
                }
                selectedFiles.clear();

            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            curDirectory = curDirectory.parent_path();
            selectedFiles.clear();
        }
        ImGui::Text("...");
        ImGui::NextColumn();
    }*/

	
    
    for (auto& directoryEntry : std::filesystem::directory_iterator(curDirectory))
    {
        const auto& path = directoryEntry.path();
        auto relativePath=std::filesystem::relative(directoryEntry.path(), assetsDirectory);
        std::string  fileNameStr = relativePath.filename().string();
        std::string fileExtension =relativePath.extension().string();

        if (fileExtension == ".meta")
            continue;

        ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
        ImVec2 uv1 = ImVec2(128.0f / 128.0f, 128.0f / 128.0f);    // UV coordinates for (32,32) in our texture
        ImVec4 bg = bg_col;
        if (selectedFiles.count(directoryEntry) > 0)
            bg = bg_col_selected;

        ImGui::ImageButton(directoryEntry.path().string().data(),iconSRV[cb_extensionsDB[fileExtension]],
            ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 0), ImVec2(1, 1),bg, tint_col);

        if (areaSelected)
        {
            auto cursorPos = ImGui::GetCursorScreenPos();

            if (cursorPos.x > std::min(selectionStart.x,selectionEnd.x) && cursorPos.x < std::max(selectionEnd.x,selectionStart.x) &&
                cursorPos.y >std::min(selectionStart.y, selectionEnd.y) && cursorPos.y< std::max(selectionStart.y, selectionEnd.y))
                selectedFiles.insert(directoryEntry);
        }


        if (ImGui::BeginDragDropSource())
        {
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
                selectedFiles.clear();
          
            }
            ImGui::EndDragDropTarget();
        }

       
		
        if (ImGui::IsItemHovered() )
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
                    selectedFiles.insert(directoryEntry);
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

        
        
        if (directoryEntry!=renaimingFileName)
            ImGui::Text(fileNameStr.c_str());
        else
        {

            if (ImGui::InputText("RenamingText", renaimingFileString, IM_ARRAYSIZE(renaimingFileString), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                renaimingFileName = (renaimingFileName.remove_filename().string() + renaimingFileString);
                std::filesystem::rename(directoryEntry, renaimingFileName);
                renaimingFileName = "";
               
            }
            if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                renaimingFileName = "";
            }
            
        }
        ImGui::NextColumn();
    }
    ImGui::Columns(1);

    if (!itemWasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !wasOverPopup)
    {
        selectedFiles.clear();
    }

    auto end = ImGui::GetContentRegionMax().y;
    auto scrll = ImGui::GetScrollY();
    auto widgetEndPos = ImVec2(widgetStartPos.x + ImGui::GetContentRegionMax().x, widgetStartPos.y + ImGui::GetContentRegionMax().y+ ImGui::GetScrollY());

    bool isWidgetHovered = io.MousePos.x > widgetStartPos.x && io.MousePos.x < widgetEndPos.x&& io.MousePos.y>widgetStartPos.y-20 && io.MousePos.y < widgetEndPos.y;
    //areaSelected = SelectionRect(&selectionEnd, &selectionStart, isWidgetHovered);
    //Popups!

  

    if (io.MouseClicked[1] && isWidgetHovered )
    {
        ImGui::OpenPopup("ContentPopUp");
    }

    ProcessPopUp();
    
    ImGui::EndChild();
    ImGui::End();
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
                    ResourceHelper::ConstructFile(curDirectory.string() + "/NewFolder");
                    break;
                case 1:
                    ResourceHelper::ConstructFile(curDirectory.string() + "/Material.mat");
                    break;
                }
            break;
        case MainPopup:
            switch (selected_Popup)
            {
                case 0:
                    for(auto& selectedFile : selectedFiles)
                        ResourceHelper::RemoveFile(selectedFile);
                    break;
                case 1:
                    renaimingFileName = *selectedFiles.begin();
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

void HudContentBrowserSystem::DrawTreeFolderWindow()
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
        

        //if (directoryEntry==curDirectory)
        //    treeFlags |= ImGuiTreeNodeFlags_Selected;
        
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

void HudContentBrowserSystem::PrintDirectory()
{
    
}

