#pragma once
#include "../Core/ECS/SystemBase.h"
#include "InputSystem.h"
#include "EditorCameraSystem.h"
#include "EditorBillboardSystem.h"
#include "TransformSystem.h"
#include "MeshSystem.h"
#include "LightSystem.h"
#include "PhysicsSystem.h"
#include "HardwareInitSystem.h"
#include "MaterialUpdateSystem.h"
#include "ResourceSystem.h"
#include "ErrorLoggingSystem.h"
#include "Rendering/HUD/HudPropertiesSystem.h"
#include "Rendering/HUD/HudContentBrowserSystem.h"
#include "Rendering/HUD/HudHierarchySystem.h"
#include "Rendering/HUD/HudPostRenderSystem.h"
#include "Rendering/HUD/HudViewportSystem.h"
#include "Rendering/HUD/HudPreRenderSystem.h"
#include "Rendering/HUD/HudConsoleSystem.h"
#include "Rendering/RenderPipeline/ShadowRenderSystem.h"
#include "Rendering/PreRenderSystem.h"
#include "Rendering/RenderInitSystem.h"
#include "Rendering/ImageBasedLightingSystem.h"
#include "Rendering/RenderPipeline/SkyboxRenderSystem.h"
#include "Rendering/RenderPipeline/OpaqueRenderSystem.h"
#include "Rendering/RenderPipeline/LightRenderSystem.h"
#include "Rendering/RenderPipeline/EditorBillboardRenderSystem.h"
#include "Rendering/RenderPipeline/ShadowMapGenerationSystem.h"
#include "Rendering/RenderPipeline/ToneMappingRenderSystem.h"
#include "Rendering/PostViewportRenderSystem.h"
#include "Rendering/RenderPipeline/EditorGridRenderSystem.h"
#include "Rendering/RenderPipeline/HbaoRenderSystem.h"

#include "../Systems/PrepareEventsSystem.h"