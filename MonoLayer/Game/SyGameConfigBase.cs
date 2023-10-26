using System.Collections.Generic;

namespace SyEngine.Game
{
public abstract class SyGameConfigBase
{
	private List<SyEcsSystemBase> _systems;

	public List<SyEcsSystemBase> GetSystems()
	{
		_systems = CreateSystems();
		return _systems;
	}
	protected abstract List<SyEcsSystemBase> CreateSystems();
}
}