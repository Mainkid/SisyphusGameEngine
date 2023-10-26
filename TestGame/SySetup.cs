using System.Collections.Generic;
using SyEngine.Game;
using TestGame;

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