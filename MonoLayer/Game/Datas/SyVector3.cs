using System.Runtime.InteropServices;

namespace SyEngine.Game.Datas
{
[StructLayout(LayoutKind.Sequential)]
public struct SyVector3
{
	public float X;
	public float Y;
	public float Z;

	public static SyVector3 One { get; } = new SyVector3 { X = 1, Y = 1, Z = 1 };
}
}