using System;
using System.Collections.Generic;
using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerEnum<T> : SerializerBase<T> where T: Enum
{
	private readonly T[] _values;
	
	public SerializerEnum()
	{
		_values = (T[])Enum.GetValues(typeof(T));
	}

	public override JSONNode Serialize(T obj)
	{
		for (var i = 0; i < _values.Length; i++)
			if (EqualityComparer<T>.Default.Equals(_values[i], obj))
				return new JSONNumber(i);
		return new JSONNumber(0);
	}

	public override T Deserialize(JSONNode json)
	{
		int i = json.AsInt;
		return i >= 0 && i < _values.Length ? _values[i] : default;
	}
}
}