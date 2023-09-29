#pragma once
#include "ISystem.h"
#include "SimpleMath.h"

class StartUpSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:

};

