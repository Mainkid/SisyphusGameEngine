using System.Collections.Generic;
using SyEngine.Core;

namespace SyEngine.Game
{
public abstract class SyGameConfigBase
{
	public abstract List<SyEcsSystemBase> GetSystems();
}
}