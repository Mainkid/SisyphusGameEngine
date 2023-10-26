using System.Runtime.InteropServices;

namespace SyEngine.Game.Comps
{
[StructLayout(LayoutKind.Sequential)]
public struct MeshComp
{
	public string TexturePath;
	public string ModelPath;
}
}