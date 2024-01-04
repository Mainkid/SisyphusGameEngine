using System;
using System.Collections.Generic;

namespace SyEngine.Helpers
{
public static class CollectionsExt
{
	public static T Last<T>(this List<T> list)
	{
		return list[list.Count - 1];
	}

	public static void PopLast<T>(this List<T> list)
	{
		list.RemoveAt(list.Count-1);
	}


	public static KeyValuePair<TKey, TValue> GetPair<TKey, TValue>(
		this Dictionary<TKey, TValue>         dict,
		Predicate<KeyValuePair<TKey, TValue>> pred)
	{
		foreach (var pair in dict)
			if (pred(pair))
				return pair;
		throw new Exception("not found");
	}
	
	public static KeyValuePair<TKey, TValue> GetByIndex<TKey, TValue>(this Dictionary<TKey, TValue> dict, int idx)
	{
		var i = 0;
		foreach (var pair in dict)
			if (i++ == idx)
				return pair;
		throw new Exception($"{idx} is out of range, count: {dict.Count}");
	}
}
}