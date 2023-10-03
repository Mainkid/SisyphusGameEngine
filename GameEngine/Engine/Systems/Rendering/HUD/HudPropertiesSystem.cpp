#include "HudPropertiesSystem.h"
#include "../../EngineContext.h"
#include "../RenderContext.h"
#include "../../HardwareContext.h"
#include "../../TransformHelper.h"
#include "../../Core/ServiceLocator.h"

void HudPropertiesSystem::Init()
{
    this->windowID = "Properties";
    this->ec = ServiceLocator::instance()->Get<EngineContext>();
}

void HudPropertiesSystem::Run()
{
    ImGui::Begin(windowID.c_str());
    //Widget::Render();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
    if (io.MouseWheel > 0.0f)
        std::cout << "";

    TransformComponent* tc = ec->scene->registry.try_get<TransformComponent>(ec->selectedEntityID);

    if (tc)
    {
        auto degToRadians = [](float angle) {return angle * M_PI / 180.0f; };
        ImGui::Text("Transform");
        Vector3 vec3Dx = tc->localPosition;
        float vec3[3]{ vec3Dx.x, vec3Dx.y, vec3Dx.z };
        ImGui::DragFloat3("Translation", vec3,0.1f);
        tc->localPosition=(Vector3(vec3[0], vec3[1], vec3[2]));

        vec3Dx =TransformHelper::GetRotationDegrees(*tc);
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
        tc->localScale=(Vector3(vec3[0],vec3[1], vec3[2]));
    }

    LightComponent* lc = ec->scene->registry.try_get<LightComponent>(ec->selectedEntityID);

    if (lc)
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light"))
        {
            // Picker
            Vector4 vec4D = lc->color;
            float vec4[4] = { vec4D.x, vec4D.y, vec4D.z, vec4D.w };
            ImGui::ColorPicker4("Color", vec4);
            lc->color = Vector4(vec4[0], vec4[1], vec4[2], vec4[3]);


            // Attenuation Radius
            vec4D = lc->paramsRadiusAndAttenuation;
            float radius = vec4D.x;
            ImGui::DragScalar("Attenuation Radius", ImGuiDataType_Float, &radius, 1.f);
            lc->paramsRadiusAndAttenuation = (Vector4(radius, vec4D.y, vec4D.z, vec4D.w));

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void HudPropertiesSystem::Destroy()
{
    
}


