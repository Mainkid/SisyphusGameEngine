#include "TransformSystem.h"


void TransformSystem::Init()
{
	
}

void TransformSystem::Run()
{
	for (auto& entity : EngineCore::instance()->scene->registry.view<TransformComponent>())
	{
		TransformComponent& tc = EngineCore::instance()->scene->registry.get<TransformComponent>(entity);
		if (tc.prevPos != tc.position);
		{
			tc.position = tc.prevPos;
			UpdateTranslationMatrix(tc, tc.position);
		}
		if (tc.prevRot != tc.rotation)
		{
			tc.rotation = tc.prevRot;
			UpdateRotationMatrix(tc, tc.rotation);
		}
		if (tc.prevScale != tc.scale)
		{
			tc.scale = tc.prevScale;
			UpdateScaleMatrix(tc, tc.scale);
		}
	}
}

void TransformSystem::Destroy()
{

}

void TransformSystem::UpdateScaleMatrix(TransformComponent& tc,Vector3& scale)
{
	tc.scaleMatrix = Matrix::CreateScale(scale);
}

void TransformSystem::UpdateRotationMatrix(TransformComponent& tc, Vector3& rot)
{
	tc.rotationMatrix = Matrix::CreateFromYawPitchRoll(rot);
}

void TransformSystem::UpdateTranslationMatrix(TransformComponent& tc, Vector3& pos)
{
	tc.translationMatrix = Matrix::CreateTranslation(pos);
}
