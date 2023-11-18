using System.Collections.Generic;
using SyEngine.Core;
using SyEngine.Game;
using TestGame;

// ReSharper disable once CheckNamespace
public class SyGameConfig : SyGameConfigBase
{
	public override List<SyEcsSystemBase> GetSystems()
	{
		return new List<SyEcsSystemBase>()
		{
			new TestSystem()
		};
	}
}