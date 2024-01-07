using System;

namespace SyEngine.Helpers
{
public static class TypesExt
{
	public static bool TryExtractGeneric(this Type type, out Type defType, out Type genType)
	{
		if (!type.IsGenericType)
		{
			defType = null;
			genType  = null;
			return false;
		}
		defType = type.GetGenericTypeDefinition();
		genType  = type.GetGenericArguments()[0];
		return true;
	}

	public static bool IsDotNetType(this Type type)
		=> type.Namespace?.StartsWith("System") ?? false;
}
}