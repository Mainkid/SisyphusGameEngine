#pragma once
#include "ISystem.h"
#include "SimpleMath.h"



struct InputComponent;

class InputSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void ProcessInput();
};

