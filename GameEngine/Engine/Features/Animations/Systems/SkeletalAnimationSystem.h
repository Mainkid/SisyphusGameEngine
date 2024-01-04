#pragma once

#include "../../Core/ECS/SystemBase.h"
#include <memory>
#include <SimpleMath.h>
#include "../SkeletalAnimation.h"
#include "../../Resources/ResourceService.h"
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>

class AnimatorComponent;
struct EngineContext;
struct HardwareContext;
struct RenderContext;
struct ResourceService;

class SkeletalAnimationSystem: public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	HardwareContext* hc;
	EngineContext* ec;
	RenderContext* rc;
	ResourceService* rs;

	boost::hash<boost::uuids::uuid> hasher;

	void CalculateBoneTransform(AnimatorComponent& animator, const AssimpNodeData* node, DirectX::SimpleMath::Matrix parentTransform);
	void UpdateAnimation(AnimatorComponent& animator);
	Bone* FindBone(const std::string& name, SkeletalAnimation* animation);
	void UpdateBone(Bone* bone, float animationTime);

	DirectX::SimpleMath::Matrix InterpolatePosition(Bone* bone, float animationTime);
	DirectX::SimpleMath::Matrix InterpolateRotation(Bone* bone, float animationTime);
	DirectX::SimpleMath::Matrix InterpolateScaling(Bone* bone, float animationTime);

	int GetPositionIndex(Bone* bone, float animationTime);
	int GetRotationIndex(Bone* bone, float animationTime);
	int GetScaleIndex(Bone* bone, float animationTime);

	float GetScaleFactor(Bone* bone, float lastTimeStamp, float nextTimeStamp, float animationTime);

};

