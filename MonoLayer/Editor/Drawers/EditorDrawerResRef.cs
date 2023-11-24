using System;
using SyEngine.Core.Datas;
using SyEngine.Core.Resources;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerResRef<TResRef, TRes> : EditorDrawerBase<TResRef> 
	where TResRef: ResRef<TRes> 
	where TRes : ResBase
{
	public EditorDrawerResRef(SyProxyEditor editor) : base(editor)
	{
	}

	public override bool Draw(string name, ref TResRef val)
	{
		var resType = ResHelper.GetResType<TRes>();

		string prevUuid = val?.Uuid;
		string newUuid  = SyProxyEditor.GeDrawResField(name, resType, prevUuid);

		if (prevUuid != newUuid)
		{
			if (newUuid == null)
				val = null;
			else
				val = (TResRef)new ResRef<TRes>(newUuid);
			return true;
		}
		return false;
	}

	public override bool DrawRaw(string name, ref object rawVal)
	{
		var  val       = (TResRef)rawVal;
		bool isChanged = Draw(name, ref val);
		rawVal = val;
		return isChanged;
	}
    
	protected override TResRef DrawImpl(string name, TResRef val)
	{
		throw new NotImplementedException(GetType().Name);
	}
}
}