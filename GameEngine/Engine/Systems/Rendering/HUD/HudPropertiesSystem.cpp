#include "HudPropertiesSystem.h"
#include "../../EngineContext.h"
#include "../RenderContext.h"
#include "../../HardwareContext.h"
#include "../../TransformHelper.h"
#include "../../Core/ServiceLocator.h"

#include "GUI.h"

void HudPropertiesSystem::Init()
{
    this->windowID = "Properties";
    this->ec = ServiceLocator::instance()->Get<EngineContext>();
   
}

void HudPropertiesSystem::Run()
{
    // This should also be in GUI?
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
       
        if (GUI::CreateNewChapter("Transform", 1))
        {
            GUI::DrawInputField(tc->localPosition, "Translation");
            GUI::DrawInputField(tc->localRotation, "Rotation");
            GUI::DrawInputField(tc->localScale, "Scale");
            GUI::EndChapter();
        }

        //TODO: to take a quick look
        //if (GUI::CreateNewChapter("Demo Chapter", 1))
        //{
        //    const char* itemsName[] = { "Item 1", "Item 2", "Item 3", "Item 4" };
        //    
        //    float f = 0.1f ;
        //    GUI::DrawInputField(&f, "float f");
        //    GUI::Separate("");
        //    float F[4] = { 1.1f, 2.2f, 3.3f, 4.4f };
        //    GUI::DrawInputField(F, itemsName,4);
        //    GUI::Separate("");

        //    bool b = true;
        //    GUI::DrawCheckbox(&b,"bool b"); 
        //        GUI::HelpMarker("this is Help Marker");
        //    GUI::Separate("");
        //    bool B[] = { 0, 1, 1, 0 };    
        //    GUI::DrawCheckbox(B, itemsName, sizeof(B));
        //    GUI::Separate("this is Separate");

        //    GUI::DrawSlider(f,"Slider float 1", 0.0f, 1.0f); 
        //    f = +0.7F;
        //    GUI::DrawSlider(f,"Slider float 2", 0.0f, 1.0f); 
        //    GUI::Separate("");


        //    GUI::DrawDropDown( itemsName,"this is a Slider", 4);

        //    GUI::EndChapter();
        //} 
    }

    LightComponent* lc = ec->scene->registry.try_get<LightComponent>(ec->selectedEntityID);
    if (lc)
    {
        if (GUI::CreateNewChapter("Light", 1))
        {
            GUI::DrawColorPicker(lc->color, "Color");
            Vector4 *vec4D = &lc->paramsRadiusAndAttenuation;
            GUI::DrawInputField(&(vec4D->x), "Attenuation Radius");
            GUI::EndChapter();
        }
    }

   GUI::End();
}

void HudPropertiesSystem::Destroy()
{
    
}


