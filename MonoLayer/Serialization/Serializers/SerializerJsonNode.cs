using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerJsonNode : SerializerBase<JSONNode>
{
	public override JSONNode Serialize(JSONNode obj)
	{
		return obj;
	}

	public override JSONNode Deserialize(JSONNode json)
	{
		return json;
	}
}
}