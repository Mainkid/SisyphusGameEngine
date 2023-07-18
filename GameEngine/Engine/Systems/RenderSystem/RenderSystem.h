#pragma once

class EngineCore;

class RenderSystem
{
public:
	RenderSystem();
	void Initialize() {};
	void Update(float deltaSec);
	~RenderSystem()=default;


};

//TODO: –ассмотреть иерархию систем нормально