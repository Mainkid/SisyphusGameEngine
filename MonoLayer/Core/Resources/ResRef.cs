namespace SyEngine.Core.Resources
{
public class ResRef<T> where T: ResBase
{
	private readonly string _uuid;
	private readonly string _path;

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private T _res;
	
	internal ResRef(string uuid)
	{
		_uuid    = uuid;
		_res     = null;
	}

	public string Uuid => _uuid;
}
}