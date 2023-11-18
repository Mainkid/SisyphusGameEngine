namespace SyEngine.Core.Helpers
{
public static class HashHelper
{
	public static int Combine(int h0, int h1)
	{
		var hash = 17;
		unchecked
		{
			hash = hash * 31 + h0;
			hash = hash * 31 + h1;
		}
		return hash;
	}

	public static int Combine(int h0, int h1, int h2)
	{
		return Combine(Combine(h0, h1), h2);
	}

	public static int Combine(int h0, int h1, int h2, int h3)
	{
		return Combine(Combine(h0, h1, h2), h3);
	}

	public static int Combine(int h0, int h1, int h2, int h3, int h4)
	{
		return Combine(Combine(h0, h1, h2, h3), h4);
	}
}
}