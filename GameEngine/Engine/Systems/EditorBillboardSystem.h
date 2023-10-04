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
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	std::hash<std::string> hasher;
	EngineContext* ec;
};