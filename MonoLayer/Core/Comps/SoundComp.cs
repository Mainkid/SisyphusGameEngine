using SyEngine.Core.Helpers;
using SyEngine.Core.Resources;

namespace SyEngine.Core.Comps
{
public struct SoundComp : SyEcs.IComp
{
	public bool  IsPlaying;
	public bool  IsLooping;
	public float Volume;
	public bool  Is3d;

	public ResRef<ResSound> Sound;

	internal int? Hash;
    
	
	public override int GetHashCode()
		=> HashHelper.Combine(IsPlaying, IsLooping, Volume, Is3d, Sound);
}
}