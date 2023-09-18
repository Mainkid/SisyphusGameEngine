#pragma once
#include "ISystem.h"
#include "SimpleMath.h"



struct InputComponent;

class InputSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	void ProcessInput();
};

