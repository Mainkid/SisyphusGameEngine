#pragma once
#include "CameraComponent.h"
#include "../Features/Physics/Components/ColliderComponent.h"
#include "EditorBillboardComponent.h"
#include "GameObjectComp.h"
#include "ImageBasedLightingComponent.h"
#include "../Features/Lighting/Components/LightComponent.h"
#include "../Features/Mesh/Components/MeshComponent.h"
#include "../Features/Particles/Components/ParticleComponent.h"
#include "../Features/Physics/Components/RBodyComponent.h"
#include "SkyboxComponent.h"
#include "TransformComponent.h"

template<typename... Types>
struct Typelist {};

using ComponentsTypelist = Typelist <CameraComponent,
	SyPrimitiveColliderComponent,
	SyTrimeshColliderComponent,
	EditorBillboardComponent,
	GameObjectComp,
	ImageBasedLightingComponent,
	LightComponent,
	MeshComponent,
	//ParticleComponent,
	SyRigidBodyComponent,
	//SkyboxComponent,
	TransformComponent
>;

inline ComponentsTypelist COMPONENTS_TYPELIST;
