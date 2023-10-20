#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"



struct InputComponent;

class InputSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void ProcessInput();
};

