#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include <typeindex>
#include <typeinfo>
#include "../Components/EditorBillboardComponent.h"


class EditorBillboardSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	std::hash<std::string> hasher;
};


