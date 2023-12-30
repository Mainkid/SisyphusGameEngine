#include "SkeletalAnimationSystem.h"

#include "../../Core/ServiceLocator.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/RenderContext.h"
#include "../../Mesh/Components/MeshComponent.h"
#include "../../Animations/Components/AnimatorComponent.h"
#include "../../../Events/SyAnimatorComponentAdded.h"
#include "../../Mesh/MeshLoader.h"
#include "../../Mesh/Components/MeshComponent.h"
#include "../../../Events/SyPlayModeEndedEvent.h"
#include "../../../Events/SyPlayModeStartedEvent.h"


SyResult SkeletalAnimationSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    rs = ServiceLocator::instance()->Get<ResourceService>();

    return SyResult();
}

SyResult SkeletalAnimationSystem::Run()
{
    auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyPlayModeEndedEvent);
    if (eventView.begin() != eventView.end())
    {
        auto View = _ecs->view<AnimatorComponent>();
        for (auto& ent : View)
        {
            auto& animator = _ecs->get<AnimatorComponent>(ent);
            animator.m_CurrentTime = 0.0f;
            animator.m_DeltaTime = 0.0f;
            CalculateBoneTransform(animator, &animator.m_CurrentAnimation->m_RootNode, Matrix::Identity);
            animator.state = AnimatorComponent::EAnimState::Disabled;
        }
    }

    auto pauseEventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyPauseModeEvent);
    if (pauseEventView.begin() != pauseEventView.end())
    {
        auto View = _ecs->view<AnimatorComponent>();
        for (auto& ent : View)
        {
            auto& animator = _ecs->get<AnimatorComponent>(ent);
            animator.state = AnimatorComponent::EAnimState::Paused;
        }
    }

    auto playmodeEventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyPlayModeStartedEvent);
    if (playmodeEventView.begin() != playmodeEventView.end())
    {
        auto View = _ecs->view<AnimatorComponent>();
        for (auto& ent : View)
        {
            auto& animator = _ecs->get<AnimatorComponent>(ent);
            animator.state = AnimatorComponent::EAnimState::Playing;
        }
    }



    auto viewEvents = _ecs->view<SyAnimatorComponentAdded>();

    //Loading bone map
    for (auto& eventID : viewEvents)
    {
        auto& eventComp = viewEvents.get<SyAnimatorComponentAdded>(eventID);
        MeshComponent& meshComp = _ecs->get<MeshComponent>(eventComp.id);
        MeshLoader::LoadAnimation(rs->FindFilePathByUUID(meshComp.modelUUID), meshComp.model->m_BoneInfoMap);
        
        std::cout << " ";
    }


    auto view = _ecs->view<MeshComponent,AnimatorComponent>();
    for (auto& entity : view)
    {
        AnimatorComponent& animator = view.get<AnimatorComponent>(entity);
        MeshComponent& meshComp = view.get<MeshComponent>(entity);

        uint32_t _hash = hasher(animator.animationUUID);
        if (_hash != animator.hash)
        {
            animator.hash = _hash;
            if (animator.animationUUID != boost::uuids::uuid())
            {
                animator.m_CurrentAnimation = rs->LoadAnimationFromFile(rs->FindFilePathByUUID(animator.animationUUID));
                animator.m_CurrentAnimation->m_BoneInfoMap = meshComp.model->m_BoneInfoMap;
                animator.m_CurrentTime = 0.0f;
                animator.m_DeltaTime = 0.0f;
                AnimatorComponent::EAnimState prevAnimState = animator.state;
                CalculateBoneTransform(animator, &animator.m_CurrentAnimation->m_RootNode, Matrix::Identity);
                animator.state = prevAnimState;
                
            }
        }
        
        if (animator.state == AnimatorComponent::Playing)
            UpdateAnimation(animator);
    }

    return SyResult();
}

SyResult SkeletalAnimationSystem::Destroy()
{
    return SyResult();
}

void SkeletalAnimationSystem::CalculateBoneTransform(AnimatorComponent& animator, const AssimpNodeData* node, DirectX::SimpleMath::Matrix parentTransform)
{
    std::string nodeName = node->name;
    Matrix nodeTransform = node->transformation;

    Bone* Bone = FindBone(nodeName,animator.m_CurrentAnimation.get());

    if (Bone)
    {
        UpdateBone(Bone, animator.m_CurrentTime);
        nodeTransform = Bone->m_LocalTransform;
    }
    else
    {
        std::cout << std::endl;
    }
    //!!!!!!
    
    Matrix globalTransformation;
    globalTransformation = nodeTransform * parentTransform;


    auto boneInfoMap = animator.m_CurrentAnimation->m_BoneInfoMap;
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        Matrix offset = boneInfoMap[nodeName].offset;
        //!!!!!!
        animator.m_FinalBoneMatrices[index] = offset * globalTransformation;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(animator,&node->children[i], globalTransformation);

}

void SkeletalAnimationSystem::UpdateAnimation(AnimatorComponent& animator)
{
    animator.m_DeltaTime = ec->deltaTime;
    if (animator.m_CurrentAnimation)
    {
        animator.m_CurrentTime += animator.m_CurrentAnimation->m_TicksPerSecond * ec->deltaTime;
        if (!animator.IsLooping && animator.m_CurrentTime > animator.m_CurrentAnimation->m_Duration)
            return;

        animator.m_CurrentTime = fmod(animator.m_CurrentTime, animator.m_CurrentAnimation->m_Duration);
        CalculateBoneTransform(animator,&animator.m_CurrentAnimation->m_RootNode, Matrix::Identity);
    }
}

Bone* SkeletalAnimationSystem::FindBone(const std::string& name, SkeletalAnimation* animation)
{
    auto iter = std::find_if(animation->m_Bones.begin(), animation->m_Bones.end(),
        [&](const Bone& Bone)
        {
            return Bone.m_Name == name;
        }
    );
    if (iter == animation->m_Bones.end()) 
        return nullptr;
    else return &(*iter);
}

void SkeletalAnimationSystem::UpdateBone(Bone* bone, float animationTime)
{
    Matrix translation = InterpolatePosition(bone,animationTime);
    Matrix rotation = InterpolateRotation(bone, animationTime);
    Matrix scale = InterpolateScaling(bone, animationTime);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    bone->m_LocalTransform = scale*rotation*translation;
}

DirectX::SimpleMath::Matrix SkeletalAnimationSystem::InterpolatePosition(Bone* bone, float animationTime)
{
    if (1 == bone->m_NumPositions)
        return  Matrix::CreateTranslation(bone->m_Positions[0].position);

    int p0Index = GetPositionIndex(bone, animationTime);

    //return Matrix::CreateTranslation(bone->m_Positions[p0Index].position);

    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(bone,bone->m_Positions[p0Index].timeStamp,
        bone->m_Positions[p1Index].timeStamp, animationTime);
    Vector3 finalPosition = Vector3::Lerp(bone->m_Positions[p0Index].position,
        bone->m_Positions[p1Index].position, scaleFactor);
    return Matrix::CreateTranslation(finalPosition);
}

DirectX::SimpleMath::Matrix SkeletalAnimationSystem::InterpolateRotation(Bone* bone, float animationTime)
{
    if (1 == bone->m_NumRotations)
    {
        auto rotation = bone->m_Rotations[0].orientation;
        rotation.Normalize();
        return Matrix::CreateFromQuaternion(rotation);
    }
   
    int p0Index = GetRotationIndex(bone,animationTime);
    int p1Index = p0Index + 1;

    //return Matrix::CreateFromQuaternion(bone->m_Rotations[p0Index].orientation);

    float scaleFactor = GetScaleFactor(bone,bone->m_Rotations[p0Index].timeStamp,
        bone->m_Rotations[p1Index].timeStamp, animationTime);
    Quaternion finalRotation = Quaternion::Slerp(bone->m_Rotations[p0Index].orientation,
        bone->m_Rotations[p1Index].orientation, scaleFactor);
    finalRotation.Normalize();
    return Matrix::CreateFromQuaternion(finalRotation);
}

DirectX::SimpleMath::Matrix SkeletalAnimationSystem::InterpolateScaling(Bone* bone, float animationTime)
{
    if (1 == bone->m_NumScalings)
        return Matrix::CreateScale(bone->m_Scales[0].scale);

    int p0Index = GetScaleIndex(bone,animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(bone,bone->m_Scales[p0Index].timeStamp,
        bone->m_Scales[p1Index].timeStamp, animationTime);
    Vector3 finalScale = Vector3::Lerp(bone->m_Scales[p0Index].scale,bone-> m_Scales[p1Index].scale
        , scaleFactor);
    return Matrix::CreateScale(finalScale);
}

int SkeletalAnimationSystem::GetPositionIndex(Bone* bone, float animationTime)
{
    for (int index = 0; index < bone->m_NumPositions - 1; ++index)
    {
        if (animationTime < bone->m_Positions[index + 1].timeStamp)
            return index;
    }
    assert(0);
}

int SkeletalAnimationSystem::GetRotationIndex(Bone* bone, float animationTime)
{
    for (int index = 0; index < bone->m_NumRotations - 1; ++index)
    {
        if (animationTime < bone->m_Rotations[index + 1].timeStamp)
            return index;
    }
    assert(0);
}

int SkeletalAnimationSystem::GetScaleIndex(Bone* bone, float animationTime)
{
    for (int index = 0; index < bone->m_NumScalings - 1; ++index)
    {
        if (animationTime < bone->m_Scales[index + 1].timeStamp)
            return index;
    }
    assert(0);
}

float SkeletalAnimationSystem::GetScaleFactor(Bone* bone, float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}
