using System;
using SyEngine.Helpers;

namespace SyEngine.Datas
{
public struct SySceneEnt
{
	internal int? InternalEnt;

	public SySceneEnt(int ent)
	{
		InternalEnt = ent;
	}

	public int Entity => InternalEnt ?? throw new Exception("scene ent is not set");
    
	public override int GetHashCode()
		=> InternalEnt.GetHashCode();
}
}