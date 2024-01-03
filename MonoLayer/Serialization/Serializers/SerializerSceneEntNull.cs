using System.Collections.Generic;
using SimpleJSON;
using SyEngine.Datas;
using SyEngine.Logger;

namespace SyEngine.Serialization.Serializers
{
public class SerializerSceneEntNull : SerializerBase<SySceneEnt?>
{
	private Dictionary<int, int> _oldEntToNewEnt;
	
	public void SetContext(Dictionary<int, int> oldEntToNewEnt)
	{
		_oldEntToNewEnt = oldEntToNewEnt;
	}
	
	public override JSONNode Serialize(SySceneEnt? obj)
	{
		if (obj?.InternalEnt == null)
			return JSONNull.CreateOrGet();
		return new JSONNumber(obj.Value.InternalEnt.Value);
	}

	public override SySceneEnt? Deserialize(JSONNode json)
	{
		int oldEnt = json.AsInt;

		if (_oldEntToNewEnt != null && _oldEntToNewEnt.TryGetValue(oldEnt, out int newEnt))
			return new SySceneEnt(newEnt);
		
		SyLog.Err(ELogTag.Scene, $"failed to load scene ent field for ent: {oldEnt}");
		return null;
	}
}
}