#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "MeshLoader.h"
#include "../Components/LightComponent.h"

struct HardwareContext;
struct EngineContext;
struct RenderContext;

class LightSystem : public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	std::hash<LightComponent> hasher;
	std::vector<Vector4> GetFrustumCorners(const Matrix& view, const Matrix proj);
	std::vector<Matrix> GenerateOrthosFromFrustum(LightComponent& lc,Vector3 direction,const Matrix& view, const Matrix proj, float _far);
	void GenerateOrthoMatrix(LightComponent& lc, float width, float depthPlane, float nearPlane);
	void GenerateViewMatrix(Vector3 cameraForward, LightComponent& lc, Vector3 pos);
	void GenerateOrthoFromFrustum(LightComponent& lc,Vector3 direction, const Matrix& view, const Matrix proj);
	void InitPointLightResources(LightComponent& lc);

	HardwareContext* hc;
	RenderContext* rc;
	EngineContext* ec;
};

