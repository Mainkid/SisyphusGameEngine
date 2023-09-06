#pragma once
class ISystem
{
public:
	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void Destroy() = 0;
};

