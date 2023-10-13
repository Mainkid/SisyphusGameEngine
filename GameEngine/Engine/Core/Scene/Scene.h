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
#include "../../Components/RBodyComponent.h"
#include "../../Tools/Data/Vector.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include "SimpleMath.h"

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

	entt::entity AddGameObject(boost::uuids::uuid uuid = boost::uuids::nil_uuid(), Vector3 position_ = Vector3::Zero);
	entt::entity AddStaticBox(const SyVector3& position_, const SyVector3& rotation_ = SyVector3(), const SyVector3 scale_ = {1.0f, 1.0f, 1.0f});
	entt::entity AddDynamicBox(const SyVector3& position_, const SyVector3& rotation_ = SyVector3(), const SyVector3 scale_ = { 1.0f, 1.0f, 1.0f });
	entt::entity AddLight(LightType _lightType, Vector3 pos = Vector3::Zero);
	entt::entity AddParticleSystem();
	bool DestroyGameObject(entt::entity);


private:
	void SetParent(entt::entity sourceGameObject, entt::entity parentGameObject);
	void AddChild(entt::entity parentGameObject, entt::entity childGameObject);  
	void RemoveChild(entt::entity parentGameObject, entt::entity childGameObject);
	bool HasHierarchyCycles(entt::entity childGameObject, entt::entity parentGameObject);
	
};

