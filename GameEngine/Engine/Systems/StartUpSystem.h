#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"

class StartUpSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:

};

