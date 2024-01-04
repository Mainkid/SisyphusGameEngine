using System.Collections.Generic;
using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerList<T> : SerializerBase<List<T>>
{
	public override JSONNode Serialize(List<T>   obj)
	{
		var jArray = new JSONArray();
		foreach (var item in obj)
		{
			if (SerializeHelper.TrySerialize(item, out var jItem))
				jArray.Add(jItem);
		}
		return jArray;
	}

	public override List<T> Deserialize(JSONNode json)
	{
		var list = new List<T>();
		
		var jArray = json.AsArray;
		foreach (var jItem in jArray.Children)
		{
			if (SerializeHelper.TryDeserialize<T>(jItem, out var item))
				list.Add(item);
		}
		
		return list;
	}
}
}