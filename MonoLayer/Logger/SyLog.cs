using System;

namespace SyEngine.Logger
{
public static class SyLog
{
	private const bool IS_PRINT_IN_TERMINAL = true;
	
	public static void Debug<T>(T tag, string msg) where T: Enum
	{
		Log(0, tag, msg);
	}

	public static void Info<T>(T tag, string msg) where T : Enum
	{
		Log(1, tag, msg);
	}

	public static void Warn<T>(T tag, string msg) where T : Enum
	{
		Log(2, tag, msg);
	}

	public static void Err<T>(T tag, string msg) where T : Enum
	{
		Log(3, tag, msg);
	}

	private static void Log<T>(int level, T tag, string msg) where T : Enum
	{
		if (IS_PRINT_IN_TERMINAL)
			Console.WriteLine($"[{tag.ToString()}] {msg}");
		
		SyProxyLogger.GE_Log(level, tag.ToString(), msg);
	}
}
}