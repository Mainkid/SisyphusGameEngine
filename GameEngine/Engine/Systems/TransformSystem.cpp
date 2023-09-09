#include "TransformSystem.h"


void TransformSystem::Init()
{
	
}

void TransformSystem::Run()
{
	auto view = EngineCore::instance()->scene->registry.view<TransformComponent>();
	for (auto& entity :view)
	{
		TransformComponent& tc = view.get<TransformComponent>(entity);
		uint32_t hsh = hasher(tc);
		if (tc.hash != hsh)
		{
			UpdateTranslationMatrix(tc, tc.localPosition);
			UpdateRotationMatrix(tc, tc.localRotation);
			UpdateScaleMatrix(tc, tc.localScale);
			tc.hash = hsh;
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

