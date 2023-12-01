using System;
using System.Reflection;
using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerReflect<T> : SerializerBase<T>
{
	private readonly FieldInfo[] _fields;

	public SerializerReflect()
	{
		var type = typeof(T);
		_fields = type.GetFields(
			BindingFlags.Instance |
			BindingFlags.Public
		);
	}


	public override JSONNode Serialize(T obj)
	{
		var json = new JSONObject();
		foreach (var field in _fields)
		{
			object fieldValue = field.GetValue(obj);
			if (SerializeHelper.TrySerializeRaw(field.FieldType, fieldValue, out var jFieldValue))
				json.Add(field.Name, jFieldValue);
		}

		return json;
	}

	public override T Deserialize(JSONNode json)
	{
		var jObject = json.AsObject;
		
		object obj = Activator.CreateInstance(typeof(T));
		foreach (var field in _fields)
			if (jObject.HasKey(field.Name))
			{
				var jFieldValue = jObject[field.Name];
				if (SerializeHelper.TryDeserializeRaw(field.FieldType, jFieldValue, out object fieldValue))
					field.SetValue(obj, fieldValue);
			}

		return (T)obj;
	}
}
}