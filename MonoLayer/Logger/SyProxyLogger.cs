using System.Runtime.CompilerServices;

namespace SyEngine.Logger
{
internal class SyProxyLogger
{
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GE_Log(int rawLevel, string tag, string msg);
}

internal enum ELogTag
{
	ProxyEcs,
	ProxyGame,
	ProxyEditor,
	Ecs,
	Scene,
	Editor
}
}