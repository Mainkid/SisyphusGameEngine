using System;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Datas
{
public struct SyVector4 : IEquatable<SyVector4>
{
	public float X;
	public float Y;
	public float Z;
	public float W;

	public SyVector4(float x, float y, float z, float w)
	{
		X = x;
		Y = y;
		Z = z;
		W = w;
	}

	public override int GetHashCode()
		=> HashHelper.Combine(X, Y, Z, W);

	public bool Equals(SyVector4 other)
	{
		return X.Equals(other.X) && Y.Equals(other.Y) && Z.Equals(other.Z) && W.Equals(other.W);
	}

	public override bool Equals(object obj)
	{
		return obj is SyVector4 other && Equals(other);
	}
}
}