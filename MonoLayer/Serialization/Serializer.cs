using System;
using System.Collections.Generic;
using SyEngine.Helpers;
using SyEngine.Resources;
using SyEngine.Serialization.Serializers;
using SimpleJSON;

namespace SyEngine.Serialization
{
public static class SerializeHelper
{
	private static Dictionary<Type, ISerializer> _serializers = new Dictionary<Type, ISerializer>()
	{
		{typeof(int), new SerializerInt()},
		{typeof(bool), new SerializerBool()},
		{typeof(float), new SerializerFloat()},
		{typeof(string), new SerializerString()},
		{typeof(object), new SerializerObject()}
	};

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	public static string ToJson<T>(T obj)
	{
		return TrySerialize(obj, out var json) ? json.ToString(4) : null;
	}

	public static T FromJson<T>(string json)
	{
		if (string.IsNullOrEmpty(json))
			return default;
		var jNode = JSON.Parse(json);
		return TryDeserialize<T>(jNode, out var obj) ? obj : default;
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	internal static bool TrySerialize<T>(T val, out JSONNode json)
	{
		if (TryGetSerializer<T>(out var ser))
		{
			json = val == null ? JSONNull.CreateOrGet() : ser.Serialize(val);
			return true;
		}
		json = null;
		return false;
	}

	internal static bool TryDeserialize<T>(JSONNode json, out T val)
	{
		if (TryGetSerializer<T>(out var ser))
		{
			val = json.IsNull ? default : ser.Deserialize(json);
			return true;
		}
		val = default;
		return false;
	}

	internal static bool TrySerializeRaw(Type type, object obj, out JSONNode json)
	{
		if (TryGetSerializerRaw(type, out var ser))
		{
			json = obj == null ? JSONNull.CreateOrGet() : ser.SerializeRaw(obj);
			return true;
		}
		json = null;
		return false;
	}

	internal static bool TryDeserializeRaw(Type type, JSONNode json, out object val)
	{
		if (TryGetSerializerRaw(type, out var ser))
		{
			val = json.IsNull ? default : ser.DeserializeRaw(json);
			return true;
		}
		val = default;
		return false;
	}

	private static bool TryGetSerializer<T>(out SerializerBase<T> ser)
	{
		if (TryGetSerializerRaw(typeof(T), out var rawSer))
		{
			ser = (SerializerBase<T>)rawSer;
			return true;
		}
		ser = null;
		return false;
	}
	
	private static bool TryGetSerializerRaw(Type type, out ISerializer ser)
	{
		if (_serializers.TryGetValue(type, out ser))
			return true;

		if (type.IsEnum)
		{
			var serType = typeof(SerializerEnum<>).MakeGenericType(type);
			ser     = (ISerializer)Activator.CreateInstance(serType);
			_serializers.Add(type, ser);
			return true;
		}
		if (type.IsArray)
		{
			var serType = typeof(SerializerArray<>).MakeGenericType(type.GetElementType());
			ser = (ISerializer)Activator.CreateInstance(serType);
			_serializers.Add(type, ser);
			return true;
		}
		if (type.TryExtractGeneric(out var defType, out var genType))
		{
			if (defType == typeof(List<>))
			{
				var serType = typeof(SerializerList<>).MakeGenericType(genType);
				ser = (ISerializer)Activator.CreateInstance(serType);
				_serializers.Add(type, ser);
				return true;
			}
			if (defType == typeof(ResRef<>))
			{
				var serType = typeof(SerializerResRef<>).MakeGenericType(genType);
				ser = (ISerializer)Activator.CreateInstance(serType);
				_serializers.Add(type, ser);
				return true;
			}
		}
		if (type.IsDotNetType())
		{
			return false;
		}

		{
			var serType = typeof(SerializerReflect<>).MakeGenericType(type);
			ser = (ISerializer)Activator.CreateInstance(serType);
			_serializers.Add(type, ser);
			return true;
		}
	}
}
}