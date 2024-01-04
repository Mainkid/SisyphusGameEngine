using System;
using SyEngine.Helpers;

namespace SyEngine.Datas
{
public struct SyVector2 : IEquatable<SyVector2>
{
	public float X;
	public float Y;


	public SyVector2(float x, float y)
	{
		X = x;
		Y = y;
	}


	public SyVector2 FloorToInt() => new SyVector2((int)X, (int)Y);


	public override string ToString()
		=> $"({X}, {Y})";

	public override int GetHashCode()
		=> HashHelper.Combine(X, Y);

	public bool Equals(SyVector2 other)
	{
		return X.Equals(other.X) && Y.Equals(other.Y);
	}

	public override bool Equals(object obj)
	{
		return obj is SyVector2 other && Equals(other);
	}
}
}