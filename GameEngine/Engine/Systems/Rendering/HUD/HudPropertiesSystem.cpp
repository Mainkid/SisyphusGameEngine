#include "HudPropertiesSystem.h"
#include "../../EngineContext.h"
#include "../RenderContext.h"
#include "ResourceHelper.h"
#include "../../HardwareContext.h"
#include "../../ResourceService.h"
#include "../../TransformHelper.h"
#include "../../Core/ServiceLocator.h"
#include "json.hpp"
#include <fstream>

void HudPropertiesSystem::Init()
{
    this->windowID = "Properties";
    this->ec = ServiceLocator::instance()->Get<EngineContext>();
    this->rs = ServiceLocator::instance()->Get<ResourceService>();
}

void HudPropertiesSystem::Run()
{
    ImGui::Begin(windowID.c_str());
    //Widget::Render();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    if (ec->selectedEntityID != entt::null)
    {
        TransformComponent* tc = ec->scene->registry.try_get<TransformComponent>(ec->selectedEntityID);

        if (tc)
        {
            auto degToRadians = [](float angle) {return angle * M_PI / 180.0f; };
            ImGui::Text("Transform");
            Vector3 vec3Dx = tc->localPosition;
            float vec3[3]{ vec3Dx.x, vec3Dx.y, vec3Dx.z };
            ImGui::DragFloat3("Translation", vec3, 0.1f);
            tc->localPosition = (Vector3(vec3[0], vec3[1], vec3[2]));

            vec3Dx = TransformHelper::GetRotationDegrees(*tc);
            vec3[0] = vec3Dx.x;
            vec3[1] = vec3Dx.y;
            vec3[2] = vec3Dx.z;

            ImGui::DragFloat3("Rotation", vec3, 1.0f);
            auto vec = Vector3(vec3[0], vec3[1], vec3[2]);
            TransformHelper::DegreesToRad(vec);
            tc->localRotation = vec;

            vec3Dx = tc->localScale;
            vec3[0] = vec3Dx.x;
            vec3[1] = vec3Dx.y;
            vec3[2] = vec3Dx.z;
            ImGui::DragFloat3("Scale", vec3, 0.1f);
            tc->localScale = (Vector3(vec3[0], vec3[1], vec3[2]));
        }
    }
    else if (ec->selectedContent.uuid!="")
    {
        if (std::filesystem::exists(rs->resourceLibrary[ec->selectedContent.uuid].path))
        {
            switch (ec->selectedContent.assetType)
            {
            case EAssetType::ASSET_MATERIAL:
                DrawMaterialProperties();
                break;
            case EAssetType::ASSET_MESH:

                break;
            case EAssetType::ASSET_SOUND:

                break;
            case EAssetType::ASSET_TEXTURE:

                break;
            }

        }
        else
        {
            ec->selectedContent.assetType = EAssetType::ASSET_NONE;
            ec->selectedContent.uuid = "";
        }
    }



    ImGui::End();
}

void HudPropertiesSystem::Destroy()
{
    
}

void HudPropertiesSystem::DrawMaterialProperties()
{
    const int comboWidth = 100;
    const int dragFloatWidth = 100;
    static std::string prevSelectedUUID = "";
    using json = nlohmann::json;
    ImGui::Text("Material");
    std::string filePath=rs->FindFilePathByUUID(ec->selectedContent.uuid);

    std::string albedoTextureUUID;
    std::string specularTextureUUID;
    std::string roughnessTextureUUID;
    std::string metallicTextureUUID;
    std::string emissiveTextureUUID;
    std::string normalmapTextureUUID;
    std::string opacityTextureUUID;
    Vector4 albedoVec, specularVec, metallicVec, emissiveVec, roughnessVec;
    
    std::ifstream file;
    json fileData;

    file.open(filePath);
    file >> fileData;
    file.close();

    albedoVec = Vector4(fileData["albedoVec"][0], fileData["albedoVec"][1], fileData["albedoVec"][2], fileData["albedoVec"][3]);
    specularVec = Vector4(fileData["specularVec"][0], fileData["specularVec"][1], fileData["specularVec"][2], fileData["specularVec"][3]);
    metallicVec = Vector4(fileData["metallicVec"][0], fileData["metallicVec"][1], fileData["metallicVec"][2], fileData["metallicVec"][3]);
    emissiveVec = Vector4(fileData["emissiveVec"][0], fileData["emissiveVec"][1], fileData["emissiveVec"][2], fileData["emissiveVec"][3]);
    roughnessVec = Vector4(fileData["roughnessVec"][0], fileData["roughnessVec"][1], fileData["roughnessVec"][2], fileData["roughnessVec"][3]);

    std::vector<Vector4> vecs = { albedoVec,specularVec,roughnessVec,metallicVec,emissiveVec};
    std::vector<std::string> textureUUIDvec = { fileData["albedoTextureUUID"], fileData["specularTextureUUID"], fileData["roughnessTextureUUID"],
    fileData["metallicTextureUUID"],fileData["emissiveTextureUUID"], fileData["normalmapTextureUUID"], fileData["opacityTextureUUID"] };

    //std::sort(textureUUIDvec.begin(), textureUUIDvec.end());

    std::vector<const char*> items = rs->GetAllResourcesOfType(EAssetType::ASSET_TEXTURE);

   


    static const char* checkBoxLabels[7] = { "Use Texture1", "Use Texture2", "Use Texture3", "Use Texture4", "Use Texture5", "Use Texture6", "Use Texture7" };
    static const char* comboBoxLabels[7] = { "Albedo Texture", "Specular Texture",
        "Roughness Texture", "Metallic Texture", "Emissive Texture", "Normalmap Texture","Opacity Texture" };
    static const char* vectorLabels[7] = { "Albedo Color", "Specular Color", "Roughness Color", "Metallic Color", "Emissive Color", "Normalmap Color", "Opacity Color" };

    float vec4[4]{ albedoVec.x,albedoVec.y,albedoVec.z,albedoVec.w};
    static int texture_current[7] = {-1,-1,-1,-1,-1,-1,-1};
    static bool params[7] = {false,false,false,false,false,false,false};
    static float col2[7][4] = { 
     {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f}};

    auto setTextureByUUID = [](std::string& uuid, int i, int* texture_cur, std::vector<const char*>& items, std::vector<std::string>& textureUUIDvec) {
        auto index = std::find(items.begin(), items.end(), uuid);
        if (index != items.end())
        {
            texture_cur[i] = std::distance(items.begin(), index);
        }
        else
        {
            texture_cur[i] = -1;
        }

    };

    //Initialization
    if (prevSelectedUUID != ec->selectedContent.uuid)
    {
        for (int i = 0; i < vecs.size(); i++)
        {
            col2[i][0] = vecs[i].x;
            col2[i][1] = vecs[i].y;
            col2[i][2] = vecs[i].z;
            col2[i][3] = vecs[i].w;

            params[i] = vecs[i].w < 0;
        }

        for (int i = 0; i < textureUUIDvec.size(); i++)
        {
            setTextureByUUID(textureUUIDvec[i], i, texture_current,items,textureUUIDvec );

           
        }


    }


    for (int i = 0; i < 5; i++)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 10));
        ImGui::Checkbox(checkBoxLabels[i], &params[i]);
        ImGui::SameLine();
        ImGui::BeginDisabled(!params[i]);
        ImGui::PushItemWidth(comboWidth);
        ImGui::Combo(comboBoxLabels[i], &texture_current[i], items.data(), items.size());
        if (ImGui::BeginDragDropTarget())
        {

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
            {
                std::string uuid;
                uuid=static_cast<char*>(payload->Data);
                uuid.resize(36);
                
                if (rs->resourceLibrary[uuid].assetType == EAssetType::ASSET_TEXTURE)
                {
                    setTextureByUUID(uuid, i, texture_current, items, textureUUIDvec);
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopItemWidth();
        ImGui::EndDisabled();

        ImGui::BeginDisabled(params[i]);
        ImGui::PushItemWidth(128);
        ImGui::ColorEdit4(vectorLabels[i], col2[i]);
        ImGui::PopItemWidth();
        ImGui::EndDisabled();
        ImGui::PopStyleVar();
        ImGui::NewLine();
    }

    for (int i = 5; i < 7; i++)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 10));
        ImGui::PushItemWidth(comboWidth);
        ImGui::Combo(comboBoxLabels[i], &texture_current[i], items.data(), items.size());
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
    }

    
    ImGui::Button("APPLY");
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        json outputData = {

                {"albedoTextureUUID",items[texture_current[0]]},
                {"specularTextureUUID",items[texture_current[1]]},
                {"roughnessTextureUUID",items[texture_current[2]]},
                {"metallicTextureUUID",items[texture_current[3]]},
                {"emissiveTextureUUID",items[texture_current[4]]},
                {"normalmapTextureUUID",items[texture_current[5]]},
                {"opacityTextureUUID",items[texture_current[6]]},
                {"albedoVec",std::vector<float>{col2[0][0],col2[0][1],col2[0][2],1.f * (!params[0]) -1* (params[0])}},
                {"specularVec",std::vector<float>{col2[1][0],col2[1][1],col2[1][2],1.f * (!params[0]) - 1 * (params[1])}},
                {"roughnessVec",std::vector<float>{col2[2][0],col2[2][1],col2[2][2],1.f * (!params[0]) - 1 * (params[2])}},
                {"metallicVec",std::vector<float>{col2[3][0],col2[3][1],col2[3][2],1.f * (!params[0]) - 1 * (params[3])}},
                {"emissiveVec",std::vector<float>{col2[4][0],col2[4][1],col2[4][2],1.f * (!params[0]) - 1 * (params[4])}}
        };
            
        std::ofstream out_file;
        out_file.open(filePath);
        out_file << outputData;
        out_file.close();

    }
    prevSelectedUUID = ec->selectedContent.uuid;
}

//TODO: Зарефакторить код после изменений Дмитрия
//TODO: Нужно избежать открытия файлов каждый фрейм
//TODO: Добавить ивент для пегезагрузки материала после обновления