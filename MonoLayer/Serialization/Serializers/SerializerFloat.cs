using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerFloat : SerializerBase<float>
{
	public override JSONNode Serialize(float   obj)
	{
		return new JSONNumber(obj);
	}

	public override float Deserialize(JSONNode json)
	{
		return json.AsFloat;
	}
}
}