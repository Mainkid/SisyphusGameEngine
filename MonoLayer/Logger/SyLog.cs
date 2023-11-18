namespace SyEngine.Logger
{
public static class SyLog
{
	public static void Debug<T>(T tag, string msg) where T: System.Enum
	{
		SyProxyLogger.GE_Log(0, tag.ToString(), msg);
	}

	public static void Info<T>(T tag, string msg) where T : System.Enum
	{
		SyProxyLogger.GE_Log(1, tag.ToString(), msg);
	}

	public static void Warn<T>(T tag, string msg) where T : System.Enum
	{
		SyProxyLogger.GE_Log(2, tag.ToString(), msg);
	}

	public static void Err<T>(T tag, string msg) where T : System.Enum
	{
		SyProxyLogger.GE_Log(3, tag.ToString(), msg);
	}
}
}