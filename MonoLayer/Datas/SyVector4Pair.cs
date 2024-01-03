using System;
using SyEngine.Helpers;

namespace SyEngine.Datas
{
public struct SyVector4Pair : IEquatable<SyVector4Pair>
{
	public SyVector4 A;
	public SyVector4 B;


	public SyVector4Pair(SyVector4 a, SyVector4 b)
	{
		A = a;
		B = b;
	}

	public SyVector4Pair FloorToInt() => new SyVector4Pair(A.FloorToInt(), B.FloorToInt());
	
	public override int GetHashCode()
		=> HashHelper.Combine(A, B);

	public bool Equals(SyVector4Pair other)
	{
		return A.Equals(other.A) && B.Equals(other.B);
	}

	public override bool Equals(object obj)
	{
		return obj is SyVector4Pair other && Equals(other);
	}
}
}