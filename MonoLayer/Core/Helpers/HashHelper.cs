using System;
using System.Collections;
using System.Collections.Generic;

namespace SyEngine.Core.Helpers
{
public static class HashHelper
{
	private static int CombineHash(int h0, int h1)
	{
		var hash = 17;
		unchecked
		{
			hash = hash * 31 + h0;
			hash = hash * 31 + h1;
		}
		return hash;
	}

	public static int Hash<T>(T val)
	{
		return EqualityComparer<T>.Default.Equals(val, default) ? 0 : val.GetHashCode();
	}

	public static int HashList<T>(List<T> collection)
	{
		var hash = 0;
		if (collection != null)
			foreach (var item in collection)
				hash = CombineHash(hash, Hash(item));
		return hash;
	}

	public static int Combine<T0, T1>(T0 t0, T1 t1)
	{
		var hash = 17;
		unchecked
		{
			hash = hash * 31 + Hash(t0);
			hash = hash * 31 + Hash(t1);
		}
		return hash;
	}

	public static int Combine<T0, T1, T2>(T0 t0, T1 t1, T2 t2)
	{
		return Combine(Combine(t0, t1), t2);
	}

	public static int Combine<T0, T1, T2, T3>(T0 t0, T1 t1, T2 t2, T3 t3)
	{
		return Combine(Combine(t0, t1, t2), t3);
	}

	public static int Combine<T0, T1, T2, T3, T4>(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4)
	{
		return Combine(Combine(t0, t1, t2, t3), t4);
	}
	
	public static int Combine<T0, T1, T2, T3, T4, T5>(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
	{
		return Combine(Combine(t0, t1, t2, t3, t4), t5);
	}
	
	public static int Combine<T0, T1, T2, T3, T4, T5, T6>(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
	{
		return Combine(Combine(t0, t1, t2, t3, t4, t5), t6);
	}
}
}