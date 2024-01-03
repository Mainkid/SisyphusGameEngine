using System;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;
using SyEngine.Resources;

namespace SyEngine.Ecs.Comps
{
public struct SoundComp : SyEcs.IComp
{
	public bool  IsPlaying;
	public bool  IsLooping;
	public float Volume;
	public bool  Is3d;

	public ResRef<ResSound> Sound;

	[Hide, NonSerialized]
	internal int? Hash;
    
	
	public override int GetHashCode()
		=> HashHelper.Combine(IsPlaying, IsLooping, Volume, Is3d, Sound);
}
}