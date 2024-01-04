#pragma once
#include "SystemBase.h"
#include "../../Systems/InputSystem.h"
#include "../../Systems/EditorCameraSystem.h"
#include "../../Systems/EditorBillboardSystem.h"
#include "../../Features/Lighting/Systems/LightSystem.h"
#include "../../Features/Mesh/Systems/MeshSystem.h"
#include "../../Features/Physics/Systems/RBSystem.h"
#include "../../Features/Physics/Systems/CollisionSystem.h"
#include "../../Features/Physics/Systems/JointSystem.h"
#include "../../Systems/HardwareInitSystem.h"
#include "../../Systems/MaterialUpdateSystem.h"
#include "../../Features/Resources/Systems/ResourceSystem.h"
#include "../../Features/ErrorLogging/Systems/ErrorLoggingSystem.h"
#include "../../Systems/SkyboxSystem.h"
#include "../../Systems/TransformSystem.h"
#include "../../Features/Particles/Systems/ParticlesSystem.h"
#include "../../Features/Rendering/HUD/HudPropertiesSystem.h"
#include "../../Features/Rendering/HUD/HudContentBrowserSystem.h"
#include "../../Features/Rendering/HUD/HudHierarchySystem.h"
#include "../../Features/Rendering/HUD/HudPostRenderSystem.h"
#include "../../Features/Rendering/HUD/HudViewportSystem.h"
#include "../../Features/Rendering/HUD/HudPreRenderSystem.h"
#include "../../Features/Rendering/HUD/HudConsoleSystem.h"
#include "../../Features/Rendering/RenderPipeline/ShadowRenderSystem.h"
#include "../../Features/Rendering/PreRenderSystem.h"
#include "../../Features/Rendering/RenderInitSystem.h"
#include "../../Features/Rendering/ImageBasedLightingSystem.h"
#include "../../Features/Rendering/RenderPipeline/SkyboxRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/OpaqueRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/LightRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/EditorBillboardRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/ShadowMapGenerationSystem.h"
#include "../../Features/Rendering/RenderPipeline/ToneMappingRenderSystem.h"
#include "../../Features/Rendering/PostViewportRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/EditorGridRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/HbaoRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/ParticleRenderSystem.h"
#include "../../Features/Rendering/RenderPipeline/EditorColliderRenderSystem.h"
#include "../../Features/Animations/Systems/SkeletalAnimationSystem.h"
#include "../../Features/Sounds/Systems/SoundSystem.h"

#include "../Mono/Systems/MonoSyncSystem.h"

#include "../../Features/Events/Systems/PrepareEventsSystem.h"
