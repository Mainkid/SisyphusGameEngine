using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerInt : SerializerBase<int>
{
	public override JSONNode Serialize(int   obj)
	{
		return new JSONNumber(obj);
	}

	public override int Deserialize(JSONNode json)
	{
		return json.AsInt;
	}
}
}