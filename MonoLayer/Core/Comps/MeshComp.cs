using SyEngine.Core.Resources;

namespace SyEngine.Core.Comps
{
public struct MeshComp : SyEcs.IComp
{
	public ResRef<ResTexture>  Texture;
	public ResRef<ResMaterial> Material;
}
}