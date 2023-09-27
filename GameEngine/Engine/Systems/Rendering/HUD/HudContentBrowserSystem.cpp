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
    ImGui::Begin("Content Browser");
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
	float panelWidth = ImGui::GetContentRegionAvail().x;
    auto widgetStartPos = ImGui::GetCursorScreenPos();
    widgetStartPos.y += ImGui::GetScrollY();
   
    bool itemWasHovered = false;

    int columnCount = max((int)panelWidth / cellSize, 1);
    ImGui::Columns(columnCount, 0, false);
    

    if (curDirectory != std::filesystem::path(assetsDirectory))
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
    }

	
    
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

      

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("_CONTENTBROWSER", nullptr, 0);
            selectedFiles.insert(directoryEntry);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            std::cout << "OK";
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
            {
                
                for (auto& item : selectedFiles)
                {
                    ResourceHelper::MoveFile_(item, directoryEntry);
                }
          
            }
            ImGui::EndDragDropTarget();
        }

        std::cout << ImGui::IsMouseDragging(ImGuiMouseButton_Left) << std::endl;
		
        if (ImGui::IsItemHovered() )
        {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    curDirectory /= path.filename();
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
            else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
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

    bool isWidgetHovered = io.MousePos.x > widgetStartPos.x && io.MousePos.x < widgetEndPos.x&& io.MousePos.y>widgetStartPos.y && io.MousePos.y < widgetEndPos.y;
    //Popups!

  

    if (io.MouseClicked[1] && isWidgetHovered )
    {
        ImGui::OpenPopup("ContentPopUp");
    }

    ProcessPopUp();
    

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
    const char* mainPopupsNames[] = { "Delete","Rename"};
    const char* createPopupNames[] = { "Folder","Material" };
    std::vector<std::function<bool(std::set<std::filesystem::path>)>> mainPredicates = {
        [](const std::set<std::filesystem::path>& set) {return set.size()>0; },
        [](const std::set<std::filesystem::path>& set) {return set.size() == 1; }
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

            }
            break;

    }

   
}



