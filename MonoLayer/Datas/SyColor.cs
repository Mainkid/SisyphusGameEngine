﻿using System;
using SyEngine.Helpers;

namespace SyEngine.Datas
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

	public SyColor(SyVector4 v)
	{
		R = v.X;
		G = v.Y;
		B = v.Z;
		A = v.W;
	}

	public SyColor WithA(float a) => new SyColor(R, G, B, a);
	
	public static SyColor White { get; } = new SyColor(1, 1, 1);
	public static SyColor Red   { get; } = new SyColor(1, 0, 0);
	public static SyColor Green { get; } = new SyColor(0, 1, 0);
	public static SyColor Blue  { get; } = new SyColor(0, 0, 1);
	
	public static implicit operator SyVector4(SyColor color)
		=> new SyVector4(color.R, color.G, color.B, color.A);

	public static implicit operator SyColor(SyVector4 v)
		=> new SyColor(v.X, v.Y, v.Z, v.W);
	
	
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