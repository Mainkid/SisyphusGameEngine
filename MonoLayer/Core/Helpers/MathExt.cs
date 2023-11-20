using System;

namespace SyEngine.Core.Helpers
{
public static class MathExt
{
	public static bool IsSame(this float a, float b)
		=> Math.Abs(a - b) <= float.Epsilon;

	public static bool NotSame(this float a, float b)
		=> Math.Abs(a - b) > float.Epsilon;
}
}