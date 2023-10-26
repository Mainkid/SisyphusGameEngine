using System.Collections.Generic;
using SyEngine.Game;
using TestGame;

// ReSharper disable once CheckNamespace
public class SyGameConfig : SyGameConfigBase
{
	protected override List<SyEcsSystemBase> CreateSystems()
	{
		return new List<SyEcsSystemBase>
		{
			new TestSystem()
		};
	}
}