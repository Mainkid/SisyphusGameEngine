using System;
using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerObject : SerializerBase<object>
{
	public override JSONNode Serialize(object   obj)
	{
		var type = obj.GetType();
		if (SerializeHelper.TrySerializeRaw(type, obj, out var jVal))
		{
			var json = new JSONObject();
			json.Add("type", type.AssemblyQualifiedName);
			json.Add("value", jVal);
			return json;
		}
		return JSONNull.CreateOrGet();
	}

	public override object Deserialize(JSONNode json)
	{
		string typeName = json["type"].Value;
		
		var type = Type.GetType(typeName, false);
		if (type == null)
		{
			Console.WriteLine($"type {typeName} not found");
			return null;
		}

		var jVal = json["value"];
		if (SerializeHelper.TryDeserializeRaw(type, jVal, out object val))
		{
			return val;
		}
		return null;
	}
}
}