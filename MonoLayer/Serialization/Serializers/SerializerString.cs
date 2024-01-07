using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerString : SerializerBase<string>
{
	public override JSONNode Serialize(string   obj)
	{
		return new JSONString(obj);
	}

	public override string Deserialize(JSONNode json)
	{
		return json.Value;
	}
}
}