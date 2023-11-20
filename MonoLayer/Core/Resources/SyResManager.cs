namespace SyEngine.Core.Resources
{
public class SyResManager
{
	public string[] FindUuidsOfType(EResourceType resType)
	{
		return SyProxyResManager.GeFindUuidsOfType(resType);
	}
	
	
	public string UuidToName(string uuid)
	{
		return SyProxyResManager.GeUuidToPath(uuid, true);
	}
	
	public string UuidToPath(string uuid)
	{
		return SyProxyResManager.GeUuidToPath(uuid, false);
	}
}
}