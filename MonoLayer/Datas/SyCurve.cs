using System;
using SyEngine.Helpers;

namespace SyEngine.Datas
{
public struct SyCurve : IEquatable<SyCurve>
{
	public SyVector2 P1;
	public SyVector2 P2;
	public SyVector4 IsUsing;
	public bool      IsReversed;


	public override int GetHashCode()
		=> HashHelper.Combine(P1, P2, IsUsing, IsReversed);

	public bool Equals(SyCurve other)
	{
		return P1.Equals(other.P1) &&
		       P2.Equals(other.P2) &&
		       IsUsing.Equals(other.IsUsing) &&
		       IsReversed == other.IsReversed;
	}

	public override bool Equals(object obj)
	{
		return obj is SyCurve other && Equals(other);
	}
}
}