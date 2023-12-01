using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerArray<T> : SerializerBase<T[]>
{
	public override JSONNode Serialize(T[] obj)
	{
		var jArray = new JSONArray();
		for (var i = 0; i < obj.Length; i++)
		{
			if (SerializeHelper.TrySerialize(obj[i], out var jItem))
				jArray.Add(jItem);
		}
		return jArray;
	}

	public override T[] Deserialize(JSONNode json)
	{
		var jArray = json.AsArray;

		var arr = new T[jArray.Count];
		for (var i = 0; i < arr.Length; i++)
		{
			if (SerializeHelper.TryDeserialize<T>(jArray[i], out var item))
				arr[i] = item;
		}
		return arr;
	}
}
}