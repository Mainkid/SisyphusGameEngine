namespace SyEngine.Core.Datas
{
public struct SyVector3
{
	public float X;
	public float Y;
	public float Z;

	public SyVector3(float x, float y, float z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	public override string ToString() => $"({X}, {Y}, {Z})";

	public static SyVector3 One { get; } = new SyVector3 { X = 1, Y = 1, Z = 1 };
}
}