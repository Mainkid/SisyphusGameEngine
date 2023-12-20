#pragma once

#include "../../Core/ECS/SystemBase.h"
#include <memory>
#include <SimpleMath.h>
#include "../SkeletalAnimation.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

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

	void CalculateBoneTransform(SkeletalAnimator* animator, const AssimpNodeData* node, DirectX::SimpleMath::Matrix parentTransform);
	void UpdateAnimation(SkeletalAnimator* animator);
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

