#include "HudPropertiesSystem.h"

#include <corecrt_math_defines.h>

#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/RenderContext.h"
#include "ResourceHelper.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../Resources/ResourceService.h"
#include "../../../Systems/TransformHelper.h"
#include "../../Core/ServiceLocator.h"
#include "../../Lighting/Components/LightComponent.h"
#include "../../Particles/Components/ParticleComponent.h"
#include "../../Lighting/Components/LightComponent.h"
#include "../../Sounds/Components/FSoundComponent.h"
#include "../../Animations/Components/AnimatorComponent.h"
#include "../../../../vendor/ImGui/curve_v122.hpp"
#include "../../../Mono/SyMono.h"
#include "../../../Mono/Objects/SyMonoEditor.h"

#include "../../../../vendor/StateMachine/imgui_node_graph_test.h"
#include "../../../../vendor/StateMachine/StateMachineLayer.h"

#include "json.hpp"

#include <fstream>


SyResult HudPropertiesSystem::Init()
{
    this->windowID = "Properties";
    this->ec = ServiceLocator::instance()->Get<EngineContext>();
    this->rs = ServiceLocator::instance()->Get<ResourceService>();
    rs->updateContentBrowser.AddRaw(this, &HudPropertiesSystem::UpdateHudProperties);

    _monoEditor = ServiceLocator::instance()->Get<mono::SyMono>()->GetEditor();

    return SyResult();
}


SyResult HudPropertiesSystem::Run()
{
    //ImGui::ShowDemoWindow();
    ImGui::Begin(windowID.c_str());
    //Widget::Render();
    bool tr = true;
    ShowStateMachineGraph(&tr);

    ImGui::PushItemWidth(ImGui::GetWindowContentRegionMax().x * -0.3f);

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //ImGui::ShowBezierDemo();

    if (ec->hudData.selectedEntityIDs.size() != 0)
    {
        if (_monoEditor->IsValid())
        {
            auto rawEnt = static_cast<uint32_t>(*ec->hudData.selectedEntityIDs.begin());
            _monoEditor->EgDrawEntityComps.Invoke(rawEnt);
        }

        //DrawParticleProperties(pc->RotationOverLifetime, "Rotation over lifetime", EParticleInput::EFloats);
        //DrawParticleProperties(pc->StartDelayTime, "Start delay time", EParticleInput::EFloats);
        //DrawParticleProperties(pc->StartLifeTime, "Start Lifetime", EParticleInput::EFloats);
        //DrawParticleProperties(pc->StartRotation, "Start Rotation", EParticleInput::EFloats);
        //DrawParticleProperties(pc->StartSize, "Start Size", EParticleInput::EFloats);
        //DrawParticleProperties(pc->StartSpeed, "Start Speed", EParticleInput::EFloats);
        //DrawParticleProperties(pc->StartColor, "Start Color", EParticleInput::EVectors);
        //DrawParticleProperties(pc->SizeOverLifetime, "Size over lifetime", EParticleInput::ECurve);
        //DrawParticleProperties(pc->SpeedOverLifetime, "Speed over lifetime", EParticleInput::ECurve);

        /*
        TransformComponent* tc = _ecs->try_get<TransformComponent>(*ec->hudData.selectedEntityIDs.begin());
        LightComponent* lc = _ecs->try_get<LightComponent>(*ec->hudData.selectedEntityIDs.begin());
        ParticleComponent* pc = _ecs->try_get<ParticleComponent>(*ec->hudData.selectedEntityIDs.begin());
        FSoundComponent* sc = _ecs->try_get<FSoundComponent>(*ec->hudData.selectedEntityIDs.begin());
        AnimatorComponent* ac = _ecs->try_get<AnimatorComponent>(*ec->hudData.selectedEntityIDs.begin());


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

        if (lc)
        {
            ImGui::Text("Light");
            Vector4 vec4D = lc->ParamsRadiusAndAttenuation;
            Vector4 color = lc->Color;
            float col3[3] = { lc->Color.x,lc->Color.y,lc->Color.z };
            ImGui::PushItemWidth(100);
            ImGui::ColorEdit3("Light Color", col3);
            ImGui::DragFloat("Light Intensity", &lc->Color.w);

            switch (lc->LightType)
            {

            case ELightType::PointLight:
                ImGui::DragFloat("Light Radius", &lc->ParamsRadiusAndAttenuation.x);
                ImGui::Checkbox("Bake Shadows", &lc->CastShadows);
                break;
            }
            ImGui::PopItemWidth();
            lc->Color.x = col3[0];
            lc->Color.y = col3[1];
            lc->Color.z = col3[2];
        }*/

        
        if (ac)
        {
            ImGui::Text("Animator");
            std::vector<boost::uuids::uuid> items = rs->GetAllResourcesOfType(EAssetType::ASSET_ANIMATION);
            std::vector<std::string> filePathsStr = rs->GetAllResourcesFilePaths(EAssetType::ASSET_ANIMATION);
            std::vector<const char*> filePaths;

            static int selectedItem = 0;

            for (int i = 0; i < filePathsStr.size(); i++) {
                if (filePathsStr[i] != "")
                    filePaths.push_back(filePathsStr[i].c_str());
            }ImGui::Combo("Filepaths", &selectedItem, filePaths.data(), filePaths.size());
            ac->animationUUID = items[selectedItem];

            ImGui::Checkbox("Is Looping", &ac->IsLooping);
        }
    }
    else if (ec->hudData.selectedContent.uuid!=boost::uuids::nil_uuid())
    {
        if (std::filesystem::exists(rs->resourceLibrary[ec->hudData.selectedContent.uuid].path))
        {
            switch (ec->hudData.selectedContent.assetType)
            {
            case EAssetType::ASSET_MATERIAL:
                DrawMaterialProperties();
                break;
            case EAssetType::ASSET_MESH:
                DrawMeshProperties();
                break;
            case EAssetType::ASSET_SOUND:

                break;
            case EAssetType::ASSET_TEXTURE:
                DrawTextureProperties();
                break;
            case EAssetType::ASSET_CUBEMAP:
                DrawTextureProperties();
                break;
            }

        }
        else
        {
            ec->hudData.selectedContent.assetType = EAssetType::ASSET_NONE;
            ec->hudData.selectedContent.uuid = boost::uuids::nil_uuid();
        }
    }



    ImGui::End();
    return SyResult();
}

SyResult HudPropertiesSystem::Destroy()
{
    return SyResult();
}

void HudPropertiesSystem::DrawMaterialProperties()
{
    const int comboWidth = 100;
    const int dragFloatWidth = 100;
    
    using json = nlohmann::json;
    ImGui::Text("Material");
    std::string filePath=rs->FindFilePathByUUID(ec->hudData.selectedContent.uuid);

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
    std::vector<boost::uuids::uuid> textureUUIDvec = {
        boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["albedoTextureUUID"]),
        boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["specularTextureUUID"]),
        boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["roughnessTextureUUID"]),
        boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["metallicTextureUUID"]),
        boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["emissiveTextureUUID"]),
        boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["normalmapTextureUUID"]),
        boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["opacityTextureUUID"]) };

    

    std::vector<boost::uuids::uuid> items = rs->GetAllResourcesOfType(EAssetType::ASSET_TEXTURE);
    std::vector<std::string> filePathsStr = rs->GetAllResourcesFilePaths(EAssetType::ASSET_TEXTURE);
    std::vector<const char*> filePaths;

    for (int i = 0; i < filePathsStr.size(); i++) {
        if (filePathsStr[i]!="")
            filePaths.push_back(filePathsStr[i].c_str());
    }
   


    static const char* checkBoxLabels[7] = { "Use Texture1", "Use Texture2", "Use Texture3", "Use Texture4", "Use Texture5", "Use Texture6", "Use Texture7" };
    static const char* comboBoxLabels[7] = { "Albedo Texture", "Specular Texture",
        "Roughness Texture", "Metallic Texture", "Emissive Texture", "Normalmap Texture","Opacity Texture" };
    static const char* vectorLabels[7] = { "Albedo Color", "Specular Color", "Roughness Color", "Metallic Color", "Emissive Color", "Normalmap Color", "Opacity Color" };

    float vec4[4]{ albedoVec.x,albedoVec.y,albedoVec.z,albedoVec.w};
    static int texture_current[7] = {-1,-1,-1,-1,-1,-1,-1};
    static bool params[7] = {true,true,true,true,true,true,true};
    static float col2[7][4] = { 
     {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f}};

    auto setTextureByUUID = [](boost::uuids::uuid uuid, int i, int* texture_cur, std::vector<boost::uuids::uuid>& items, std::vector<boost::uuids::uuid>& textureUUIDvec) {
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
    if (prevSelectedUUID != ec->hudData.selectedContent.uuid)
    {
        for (int i = 0; i < vecs.size(); i++)
        {
            col2[i][0] = vecs[i].x;
            col2[i][1] = vecs[i].y;
            col2[i][2] = vecs[i].z;
            col2[i][3] = vecs[i].w;
            col2[i][3] = 1.0f;
            params[i] = vecs[i].w < 0;
        }

        params[5] = vecs[2].y < 0;

        for (int i = 0; i < textureUUIDvec.size(); i++)
        {
            setTextureByUUID(textureUUIDvec[i], i, texture_current,items,textureUUIDvec );
        }


    }

    auto dragDropFunc = [&](int i) {
        if (ImGui::BeginDragDropTarget())
        {

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
            {
                std::string uuid;
                uuid = static_cast<char*>(payload->Data);
                uuid.resize(36);
                if (rs->resourceLibrary[boost::lexical_cast<boost::uuids::uuid>(uuid)].assetType == EAssetType::ASSET_TEXTURE)
                {
                    setTextureByUUID(boost::lexical_cast<boost::uuids::uuid>(uuid), i, texture_current, items, textureUUIDvec);
                }
            }

            ImGui::EndDragDropTarget();
        }
    };

    for (int i = 0; i < 5; i++)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 10));
        ImGui::Checkbox(checkBoxLabels[i], &params[i]);
        ImGui::SameLine();
        ImGui::BeginDisabled(!params[i]);
        ImGui::PushItemWidth(comboWidth);
        if (texture_current[i] >= items.size())
        {
            SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Selected asset is deleted from project. Assigning default asset to this resource");
            setTextureByUUID(rs->baseResourceDB[EAssetType::ASSET_TEXTURE].uuid, i, texture_current, items, textureUUIDvec);
        }
        ImGui::Combo(comboBoxLabels[i], &texture_current[i], filePaths.data(), filePaths.size());
        dragDropFunc(i);
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
        ImGui::Checkbox(checkBoxLabels[i], &params[i]);
        ImGui::SameLine();
        ImGui::BeginDisabled(!params[i]);
        ImGui::PushItemWidth(comboWidth);
        if (texture_current[i] >= items.size())
        {
            SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Selected asset is deleted from project. Assigning default asset to this resource");
            setTextureByUUID(rs->baseResourceDB[EAssetType::ASSET_TEXTURE].uuid, i, texture_current, items, textureUUIDvec);
        }
        ImGui::Combo(comboBoxLabels[i], &texture_current[i], filePaths.data(), filePaths.size());
        dragDropFunc(i);
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        ImGui::EndDisabled();
    }

    
    
    ImGui::Button("APPLY");
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        for (int i = 0; i < 7; i++)
        {
            if (texture_current[i] >= items.size())
            {
                SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Selected asset is deleted from project. Assigning default asset to this resource");
                setTextureByUUID(rs->baseResourceDB[EAssetType::ASSET_TEXTURE].uuid, i, texture_current, items, textureUUIDvec);
            }
        }

        json outputData = {

                {"albedoTextureUUID",boost::lexical_cast<std::string>(items[texture_current[0]])},
                {"specularTextureUUID",boost::lexical_cast<std::string>(items[texture_current[1]])},
                {"roughnessTextureUUID",boost::lexical_cast<std::string>(items[texture_current[2]])},
                {"metallicTextureUUID",boost::lexical_cast<std::string>(items[texture_current[3]])},
                {"emissiveTextureUUID",boost::lexical_cast<std::string>(items[texture_current[4]])},
                {"normalmapTextureUUID",boost::lexical_cast<std::string>(items[texture_current[5]])},
                {"opacityTextureUUID",boost::lexical_cast<std::string>(items[texture_current[6]])},
                {"albedoVec",std::vector<float>{col2[0][0],col2[0][1],col2[0][2],1.f * (!params[0]) -1* (params[0])}},
                {"specularVec",std::vector<float>{col2[1][0],col2[1][1],col2[1][2],1.f * (!params[1]) - 1 * (params[1])}},
                {"roughnessVec",std::vector<float>{col2[2][0],1.f * (!params[5]) - 1 * (params[5]),col2[2][2],1.f * (!params[2]) - 1 * (params[2])}},
                {"metallicVec",std::vector<float>{col2[3][0],col2[3][1],col2[3][2],1.f * (!params[3]) - 1 * (params[3])}},
                {"emissiveVec",std::vector<float>{col2[4][0],col2[4][1],col2[4][2],0.1f * (!params[4]) - 1 * (params[4])}}
        };
            
        std::ofstream out_file;
        out_file.open(filePath);
        out_file << outputData;
        out_file.close();

        rs->LoadResource(rs->GetUUIDFromPath(filePath),true);
    }

    prevSelectedUUID = ec->hudData.selectedContent.uuid;
}

void HudPropertiesSystem::DrawTextureProperties()
{
    
    std::vector<const char*> textureTypeStr = { "Texture 2D", "Cubemap" };
    std::vector<const char*> wrapModeStr = { "Repeat","Clamp","Mirror" };
    std::vector<const char*> filterModeStr = { "Bilinear","Trilinear","Point" };

    std::string metafilePath = rs->FindFilePathByUUID(ec->hudData.selectedContent.uuid)+".meta";
    
    std::ifstream in_file;
    in_file.open(metafilePath);
    nlohmann::json data;
    in_file >> data;

    static bool sRGB = false;
    static bool generateMipMaps = false;
    static int textureType = 0;
    static int wrapMode = 0;
    static int filterMode = 0;

    if (prevSelectedUUID != ec->hudData.selectedContent.uuid)
    {
        filterMode =static_cast<int>(data["FilterMode"]);
        textureType = static_cast<int>(data["TextureType"]);
        wrapMode = static_cast<int>(data["WrapMode"]);
        sRGB = data["sRGB"];
        generateMipMaps = data["GenerateMipMaps"];

        prevSelectedUUID = ec->hudData.selectedContent.uuid;
    }

    ImGui::Checkbox("sRGB", &sRGB);
    ImGui::Checkbox("generateMipMaps", &generateMipMaps);
    ImGui::Combo("TextureType", &textureType, textureTypeStr.data(),textureTypeStr.size());
    ImGui::Combo("FilterMode", &filterMode, filterModeStr.data(), filterModeStr.size());
    ImGui::Combo("WrapMode", &wrapMode, wrapModeStr.data(), wrapModeStr.size());

    ImGui::Button("APPLY");
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        data["sRGB"] = sRGB;
        data["generateMipMaps"] = generateMipMaps;
        data["TextureType"] = textureType;
        data["FilterMode"] = filterMode;
        data["WrapMode"] = wrapMode;

        if (textureType == 1)
        {
            rs->resourceLibrary[ec->hudData.selectedContent.uuid].assetType = EAssetType::ASSET_CUBEMAP;
            data["AssetType"] = static_cast<int>(EAssetType::ASSET_CUBEMAP);
        }
        else if (textureType == 0)
        {
            rs->resourceLibrary[ec->hudData.selectedContent.uuid].assetType = EAssetType::ASSET_TEXTURE;
            data["AssetType"] = static_cast<int>(EAssetType::ASSET_TEXTURE);
        }

        std::ofstream out_file;
        out_file.open(metafilePath);
        out_file << data;
        out_file.close();

        

        //rs->LoadResource(ec->hudData.selectedContent.uuid, true);
    }
}

void HudPropertiesSystem::DrawMeshProperties()
{
    if (ImGui::Button("Extract animations"))
    {
        std::string fileName = rs->FindFilePathByUUID(ec->hudData.selectedContent.uuid);
        std::unordered_map<std::string, BoneInfo> tmp;
        MeshLoader::LoadAnimation(fileName, tmp, true);
    }
    
}

void HudPropertiesSystem::UpdateHudProperties(bool)
{
    prevSelectedUUID = boost::uuids::nil_uuid();
}

void HudPropertiesSystem::DrawParticleProperties(ParticleInputDataF& data, std::string label, EParticleInput inputType)
{
    static std::unordered_map<std::string, EInputType> inputTypeMap = {
                {"Float",EInputType::SimpleFloat},
                {"Random Between Floats", EInputType::RandomBetweenFloats},
                {"Vector", EInputType::SimpleVector},
                {"Random Between Vectors", EInputType::RandomBetweenVectors},
                {"Curve", EInputType::SimpleCurve} };

    //ImCurve::ShowBezierDemo();
    const char* itemsFloatInputType[] = { "Float", "Random Between Floats" };
    const char* itemsVecInputType[] = { "Vector", "Random Between Vectors" };
    const char* itemsCurveInputType[] = { "Curve" };

    int floatInputIndex = static_cast<int>(data.InputType);
    int vectorInputIndex = static_cast<int>(data.InputType)-2;
    int curveInputIndex = 0;

    std::stringstream dataAddress;
    dataAddress << &data;

    ImGui::Text(label.c_str());
    if (inputType == EParticleInput::EFloats)
    {
        std::string comboLabel = "##combo" + dataAddress.str();
        ImGui::Combo(comboLabel.c_str(), &floatInputIndex, itemsFloatInputType, IM_ARRAYSIZE(itemsFloatInputType));
        if (inputTypeMap[itemsFloatInputType[floatInputIndex]] == EInputType::SimpleFloat)
        {
            std::string strLabel = "##input float" + dataAddress.str();
            ImGui::InputFloat(strLabel.c_str(), &data.Fvalue);
        }
        else if (inputTypeMap[itemsFloatInputType[floatInputIndex]] == EInputType::RandomBetweenFloats)
        {
            float vec2f[2] = { data.RandomBetweenConstsF.f1,
                 data.RandomBetweenConstsF.f2 };

            std::string strLabel = "##input float" + dataAddress.str();
            ImGui::InputFloat2(strLabel.c_str(), vec2f);

            data.RandomBetweenConstsF.f1 = vec2f[0];
            data.RandomBetweenConstsF.f2 = vec2f[1];
        }

        data.InputType = static_cast<EInputType>(floatInputIndex);
    }
    else if (inputType == EParticleInput::EVectors)
    {
        std::string comboLabel = "##combo" + dataAddress.str();
        ImGui::Combo(comboLabel.c_str(), &vectorInputIndex, itemsVecInputType, IM_ARRAYSIZE(itemsVecInputType));
        if (inputTypeMap[itemsVecInputType[vectorInputIndex]] == EInputType::SimpleVector)
        {
            float vec4f[4] = { data.V4value.x,data.V4value.y,data.V4value.z,data.V4value.w };

            std::string strLabel = "##input vector" + dataAddress.str();
            ImGui::InputFloat4(strLabel.c_str(), vec4f);

            data.V4value = Vector4(vec4f[0], vec4f[1], vec4f[2], vec4f[3]);
        }
        else if (inputTypeMap[itemsVecInputType[vectorInputIndex]] == EInputType::RandomBetweenVectors)
        {
            float vec4f_1[4] = { data.RandomBetweenConstsV.vec1.x, data.RandomBetweenConstsV.vec1.y,
            data.RandomBetweenConstsV.vec1.z, data.RandomBetweenConstsV.vec1.w};

            float vec4f_2[4] = { data.RandomBetweenConstsV.vec2.x, data.RandomBetweenConstsV.vec2.y,
             data.RandomBetweenConstsV.vec2.z,  data.RandomBetweenConstsV.vec2.w };

            std::string strLabel1 = "##input vector1" + dataAddress.str();
            std::string strLabel2 = "##input vector2" + dataAddress.str();
            ImGui::InputFloat4(strLabel1.c_str(), vec4f_1);
            ImGui::InputFloat4(strLabel2.c_str(), vec4f_2);

            data.RandomBetweenConstsV.vec1 = Vector4(vec4f_1[0], vec4f_1[1], vec4f_1[2], vec4f_1[3]);
            data.RandomBetweenConstsV.vec2 = Vector4(vec4f_2[0], vec4f_2[1], vec4f_2[2], vec4f_2[3]);
        }

        data.InputType = static_cast<EInputType>(vectorInputIndex + 2);
    }

    else if (inputType == EParticleInput::ECurve)
    {
        bool IsUsing = data.Curve.IsUsing.x;

        std::string checkboxLabel = "Use##" + dataAddress.str();
        ImGui::SameLine();
        ImGui::Checkbox(checkboxLabel.c_str(), &IsUsing);

        

        data.Curve.IsUsing = Vector4(IsUsing, IsUsing, IsUsing, IsUsing);

        if (IsUsing)
        {
            bool IsReversed = data.Curve.IsReversed;
            std::string checkboxLabel = "Is Reversed?##" + dataAddress.str();
            ImGui::Checkbox(checkboxLabel.c_str(), &IsReversed);

            data.Curve.IsReversed = IsReversed;
            data.Curve.IsUsing.w = IsReversed;

            float curve[5] = { data.Curve.P1.x,data.Curve.P1.y,
                data.Curve.P2.x,data.Curve.P2.y,0 };
            ImCurve::Bezier(dataAddress.str().c_str(), curve);

            data.Curve.P1.x = curve[0];
            data.Curve.P1.y = curve[1];
            data.Curve.P2.x = curve[2];
            data.Curve.P2.y = curve[3];
        }
        
    }

}

