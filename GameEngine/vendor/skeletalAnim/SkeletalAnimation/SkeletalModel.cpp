#include "SkeletalModel.h"

//////////////////////////////////////////////////////////////////////////
// Based on: http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html //
//////////////////////////////////////////////////////////////////////////

using namespace DirectX::SimpleMath;

namespace SA
{
	SkeletalModel::SkeletalModel() 
		: m_AnimationTime(0.0f)
	{
		Clear();
	}





	SkeletalModel::~SkeletalModel()
	{
		Clear();
	}





	void SkeletalModel::Clear()
	{
		for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		{
			delete[] m_Meshes[i].pVertices;
			delete[] m_Meshes[i].pNormals;
			delete[] m_Meshes[i].pTransformedVertices;
			delete[] m_Meshes[i].pTransformedNormals;
			delete[] m_Meshes[i].pIndices;
		}
		m_Meshes.clear();

		for (unsigned int i = 0; i < m_Skeleton.Bones.size(); ++i)
		{
			delete[] m_Skeleton.Bones[i].pWeights;
			delete[] m_Skeleton.Bones[i].pChildren;
		}
		m_Skeleton.Bones.clear();

		m_Animation.NodeAnimations.clear();
		m_Animation.Duration = 0.0f;
		m_Animation.TicksPerSecond = 0.0f;

		m_GlobalInverseTransform = Matrix::Identity;
		m_AnimationTime = 0.0f;
	}





	void SkeletalModel::Update(float a_Dt)
	{
		m_AnimationTime = fmodf(m_AnimationTime + a_Dt * m_Animation.TicksPerSecond, m_Animation.Duration);
		//

		ReadNodeHierarchy(m_AnimationTime, m_Animation, m_Skeleton, m_Skeleton.Bones[0], Matrix::Identity);
		TransformVertices(m_Skeleton);
	}





	void SkeletalModel::ReadNodeHierarchy(float AnimationTime, sAnimation& a_Animation, sSkeleton& a_Skeleton, sBone& a_Bone, const Matrix& ParentTransform)
	{
		std::string NodeName(a_Bone.Name);
		Matrix NodeTransformation(a_Bone.NodeTransform);
		const sNodeAnimation* pNewNodeAnim = FindNodeAnim(a_Animation, NodeName);

		if (pNewNodeAnim)
		{
			Vector3 Translation = NodeAnimation_FindInterpolatedPosition(*pNewNodeAnim, AnimationTime);
			Quaternion RotationQ = NodeAnimation_FindInterpolatedRotation(*pNewNodeAnim, AnimationTime);

// 			Vector3 Scaling2(1, 1, 1);
// 			Matrix ScalingM2 = glm::scale(Scaling2);

			//Matrix RotationM2 = glm::toMat4(RotationQ);

			Matrix RotationM2 = Matrix::CreateFromQuaternion(RotationQ);

			Matrix TranslationM2 = Matrix::CreateTranslation(Translation);

			// Combine the above transformations
			NodeTransformation = TranslationM2 * RotationM2;// * ScalingM2;
		}

		Matrix GlobalTransformation = ParentTransform * NodeTransformation;

		unsigned int BoneIndex = Skeleton_FindBoneIndex(a_Skeleton, NodeName);
		if (BoneIndex != -1)
		{
			sBone* pBone = &a_Skeleton.Bones[BoneIndex];
			pBone->FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * pBone->OffsetMatrix;
		}

		for (unsigned int i = 0; i < a_Bone.NumChildren; i++)
		{
			ReadNodeHierarchy(AnimationTime, a_Animation, a_Skeleton, a_Skeleton.Bones[a_Bone.pChildren[i]], GlobalTransformation);
		}
	}





	void SkeletalModel::TransformVertices(const sSkeleton& a_Skeleton)
	{
		for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		{
			// Reset mesh vertices and normals
			sAnimatedMesh& AnimMesh = m_Meshes[i];
			memset(AnimMesh.pTransformedVertices, 0, AnimMesh.NumVertices*sizeof(Vector3));
			memset(AnimMesh.pTransformedNormals, 0, AnimMesh.NumVertices*sizeof(Vector3));

			//
			for (unsigned int i = 0; i < a_Skeleton.Bones.size(); ++i)
			{
				const sBone& Bone = a_Skeleton.Bones[i];
				//
				Matrix Transformation = Bone.FinalTransformation;
				Matrix Rotation = Matrix(Transformation);
				//
				for (unsigned int i = 0; i < Bone.NumWeights; ++i)
				{
					sWeight Weight = Bone.pWeights[i];
					//
					Vector3 inVertex = AnimMesh.pVertices[Weight.VertexID];
					Vector3& outVertex = AnimMesh.pTransformedVertices[Weight.VertexID];

					Vector4 _tmp = Vector4(inVertex.x,inVertex.y,inVertex.z, 1);
					;
					outVertex += Vector3(Vector4::Transform(_tmp, Transformation) * Weight.Weight);
					//
					Vector3 inNormal = AnimMesh.pNormals[Weight.VertexID];
					Vector3& outNormal = AnimMesh.pTransformedNormals[Weight.VertexID];
					outNormal += (Vector3::Transform(inNormal,Rotation)) * Weight.Weight;
				}
			}
		}

		// Normalize normals
		for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		{
			sAnimatedMesh& AnimMesh = m_Meshes[i];
			for (unsigned int i = 0; i < AnimMesh.NumVertices; ++i)
			{
				AnimMesh.pTransformedNormals[i].Normalize();
			}
		}
	}





	const sNodeAnimation* FindNodeAnim(const sAnimation& a_Animation, const std::string& a_NodeName)
	{
		for (unsigned int i = 0; i < a_Animation.NodeAnimations.size(); ++i)
		{
			const sNodeAnimation& NodeAnim = a_Animation.NodeAnimations[i];
			if (NodeAnim.Name == a_NodeName)
			{
				return &NodeAnim;
			}
		}

		return NULL;
	}





	Vector3 InterpolateValue(const Vector3& a_Start, const Vector3& a_End, float a_Factor)
	{
		
		return Vector3::Lerp(a_Start, a_End, a_Factor);
	}





	Quaternion InterpolateValue(const Quaternion& a_Start, const Quaternion& a_End, float a_Factor)
	{
		return Quaternion::Slerp(a_Start, a_End, a_Factor);
	}





	unsigned int Skeleton_FindBoneIndex(const sSkeleton& a_Skeleton, const std::string& a_Name)
	{
		for (unsigned int i = 0; i < a_Skeleton.Bones.size(); ++i)
		{
			if (a_Skeleton.Bones[i].Name == a_Name)
				return i;
		}
		return -1;
	}





	sBone* Skeleton_FindBone(sSkeleton& a_Skeleton, const std::string& a_Name)
	{
		return &a_Skeleton.Bones[Skeleton_FindBoneIndex(a_Skeleton, a_Name)];
	}
}