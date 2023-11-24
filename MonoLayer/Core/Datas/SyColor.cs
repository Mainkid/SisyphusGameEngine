using System;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Datas
{
public struct SyColor : IEquatable<SyColor>
{
	public float R;
	public float G;
	public float B;
	public float A;

	public SyColor(float r, float g, float b)
	{
		R = r;
		G = g;
		B = b;
		A = 1;
	}
	
	public SyColor(float r, float g, float b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}
	
	
	public static SyColor White { get; } = new SyColor(1, 1, 1);
	public static SyColor Red   { get; } = new SyColor(1, 0, 0);
	public static SyColor Green { get; } = new SyColor(0, 1, 0);
	public static SyColor Blue  { get; } = new SyColor(0, 0, 1);
	
	
	public static bool operator==(SyColor a, SyColor b) 
		=> a.R.IsSame(b.R) && a.B.IsSame(b.B) && a.G.IsSame(b.G) && a.A.IsSame(b.A);

	public static bool operator !=(SyColor a, SyColor b)
		=> a.R.NotSame(b.R) || a.B.NotSame(b.B) || a.G.NotSame(b.G) && a.A.NotSame(b.A);

	public bool Equals(SyColor other) 
		=> this == other;
	
	public override bool Equals(object obj) 
		=> obj is SyColor other && this == other;
	
	public override int GetHashCode()
		=> HashHelper.Combine(R, G, B, A);
}
}