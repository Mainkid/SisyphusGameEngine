﻿using System;
using SyEngine.Helpers;

namespace SyEngine.Datas
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


	public SyVector4 FloorToInt() => new SyVector4((int)X, (int)Y, (int)Z, (int)W);
	
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