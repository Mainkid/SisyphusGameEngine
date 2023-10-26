using System.Runtime.InteropServices;

namespace SyEngine.Game.Comps
{
[StructLayout(LayoutKind.Sequential)]
public struct TimeData
{
	public float TotalTime;
	public float DeltaTime;
}
}