#pragma once

class EngineCore;

class RenderSystem
{
public:
	RenderSystem();
	static RenderSystem* instance()
	{
		static RenderSystem inst;
		return &inst;
	}

	void StartUp() {};
	void ShutDown() {};
	void Update(float deltaSec);
	~RenderSystem()=default;


};

//TODO: –ассмотреть иерархию систем нормально