#pragma once
#include "CameraComponent.h"
#include "ColliderComponent.h"
#include "EditorBillboardComponent.h"
#include "GameObjectComp.h"
#include "ImageBasedLightingComponent.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "ParticleComponent.h"
#include "RBodyComponent.h"
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
	SyRBodyComponent,
	//SkyboxComponent,
	TransformComponent
>;

inline ComponentsTypelist COMPONENTS_TYPELIST;
