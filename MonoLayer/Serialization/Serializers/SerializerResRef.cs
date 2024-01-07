using SyEngine.Resources;
using SimpleJSON;

namespace SyEngine.Serialization.Serializers
{
public class SerializerResRef<T> : SerializerBase<ResRef<T>> where T: ResBase
{
	public override JSONNode Serialize(ResRef<T>   obj)
	{
		return obj.Uuid;
	}

	public override ResRef<T> Deserialize(JSONNode json)
	{
		string uuid = json.Value;
		return new ResRef<T>(uuid);
	}
}
}