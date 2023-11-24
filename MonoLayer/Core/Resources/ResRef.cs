using SyEngine.Core.Datas;

namespace SyEngine.Core.Resources
{
public class ResRef<T> where T: ResBase
{
	private readonly string _uuid;

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private T _res;

	internal ResRef(string uuid)
	{
		_uuid = uuid;
		_res  = null;
	}

	public string Uuid => _uuid;

	
	public override int GetHashCode()
		=> _uuid.GetHashCode();
}
}