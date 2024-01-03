using System.Collections.Generic;
using SyEngine.Ecs;

namespace SyEngine.Game
{
public abstract class SyGameConfigBase
{
	public abstract List<SyEcsSystemBase> GetSystems();
}
}