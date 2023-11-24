using System;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Datas
{
public struct SyVector3 : IEquatable<SyVector3>
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
	
	
	public static bool operator==(SyVector3 a, SyVector3 b)
		=> a.X.IsSame(b.X) && a.Y.IsSame(b.Y) && a.Z.IsSame(b.Z);

	public static bool operator !=(SyVector3 a, SyVector3 b)
		=> a.X.NotSame(b.X) && a.Y.NotSame(b.Y) && a.Z.NotSame(b.Z);

	public bool Equals(SyVector3 other)
		=> this == other;

	public override bool Equals(object obj) 
		=> obj is SyVector3 other && this == other;

	public override int GetHashCode()
		=> HashHelper.Combine(X, Y, Z);
}
}