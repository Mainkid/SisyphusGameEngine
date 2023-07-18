#pragma once

class EngineCore;

class RenderSystem
{
public:
	RenderSystem() = default;
	RenderSystem(EngineCore* _engine);
	void Initialize() {};
	void Update(float deltaSec);
	~RenderSystem()=default;

	EngineCore* engine;

};

//TODO: –ассмотреть иерархию систем нормально