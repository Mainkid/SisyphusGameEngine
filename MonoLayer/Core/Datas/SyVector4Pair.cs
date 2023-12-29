using System;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Datas
{
public struct SyVector4Pair : IEquatable<SyVector4Pair>
{
	public SyVector4 A;
	public SyVector4 B;


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