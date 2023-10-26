using System.Collections.Generic;
using Leopotam.EcsLite;

namespace SyEngine.Game
{
internal class SyGameContext
{
	internal EcsWorld EcsWorld;

	internal readonly Dictionary<int, uint> GameEntToEngineEnt        = new Dictionary<int, uint>();
	internal readonly Dictionary<uint, int> EngineEntToGameEnt        = new Dictionary<uint, int>();
	internal readonly Dictionary<int, int>  GameEntToEngineCompsCount = new Dictionary<int, int>();
}
}
