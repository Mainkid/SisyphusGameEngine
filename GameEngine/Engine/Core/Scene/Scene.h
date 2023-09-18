#pragma once
#include "../../../vendor/entt/entt.hpp"
#include <vector>
#include <memory>
#include "../../Components/DataComponent.h"
#include "../../Components/MeshComponent.h"
#include "../../Components/LightComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/ParticleComponent.h"
#include "../../Components/CameraComponent.h"
#include "../../Components/EditorBillboardComponent.h"


class Scene
{
	friend class GameObject;
	friend class HierarchyWidget;
	friend class HudHierarchySystem;
public:
	
	std::set<entt::entity> gameObjects;
	entt::registry registry;
	CameraComponent* camera;
	TransformComponent* cameraTransform;

	Scene();
	~Scene() = default;
	void Initialize();
	void Update(float deltaSec);
	void Render() {};

	entt::entity AddGameObject();
	entt::entity AddLight(LightType _lightType);
	entt::entity AddParticleSystem();
	bool DestroyGameObject(entt::entity);


private:
	void SetParent(entt::entity sourceGameObject, entt::entity parentGameObject);
	void AddChild(entt::entity parentGameObject, entt::entity childGameObject);  
	void RemoveChild(entt::entity parentGameObject, entt::entity childGameObject);
	bool HasHierarchyCycles(entt::entity childGameObject, entt::entity parentGameObject);
	
};

