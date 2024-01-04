using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerBool : SerializerBase<bool>
{
	public override JSONNode Serialize(bool   obj)
	{
		return new JSONBool(obj);
	}

	public override bool Deserialize(JSONNode json)
	{
		return json.AsBool;
	}
}
}