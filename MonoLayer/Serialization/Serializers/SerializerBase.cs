using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public interface ISerializer
{
	JSONNode SerializeRaw(object   obj);
	object DeserializeRaw(JSONNode json);
}

public abstract class SerializerBase<T> : ISerializer
{
	public abstract JSONNode Serialize(T obj);
	public abstract T Deserialize(JSONNode json);


	public JSONNode SerializeRaw(object obj)
	{
		return Serialize((T)obj);
	}

	public object DeserializeRaw(JSONNode json)
	{
		return Deserialize(json);
	}
}
}