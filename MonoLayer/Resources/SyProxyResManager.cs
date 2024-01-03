using System.Runtime.CompilerServices;

namespace SyEngine.Resources
{
public class SyProxyResManager
{
	internal readonly SyResManager Manager = new SyResManager();
	
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern string[] GeFindPathsOfType(EResourceType type);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern string[] GeFindUuidsOfType(EResourceType type);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern string GePathToUuid(string path);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern string GeUuidToPath(string uuid, bool onlyFileName);
}
}