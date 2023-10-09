#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include <typeindex>
#include <typeinfo>
#include <string>

struct EngineContext;

class EditorBillboardSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	std::hash<std::string> hasher;
	EngineContext* ec;
};


