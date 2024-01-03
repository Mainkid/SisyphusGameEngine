using System;
using SyEngine.Resources;

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
		string newUuid  = prevUuid;
		SyProxyEditor.GeDrawResField(name, resType, ref newUuid);

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
}
}