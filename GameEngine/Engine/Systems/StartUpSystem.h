#pragma once
#include "ISystem.h"
#include "SimpleMath.h"

class StartUpSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:

};

