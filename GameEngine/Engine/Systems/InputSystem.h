#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"



struct InputComponent;

class InputSystem : public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	void ProcessInput();
};

