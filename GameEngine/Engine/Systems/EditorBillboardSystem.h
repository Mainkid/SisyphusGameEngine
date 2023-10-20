#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include <typeindex>
#include <typeinfo>
#include <string>

struct EngineContext;

class EditorBillboardSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	std::hash<std::string> hasher;
	EngineContext* ec;
};