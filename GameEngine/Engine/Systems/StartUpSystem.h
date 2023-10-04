#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"

class StartUpSystem : public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:

};

