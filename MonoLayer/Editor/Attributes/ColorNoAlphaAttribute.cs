using System.Collections.Generic;
using SyEngine.Datas;

namespace SyEngine.Editor.Attributes
{
public class ColorNoAlphaAttribute : EditorFieldBaseAttribute
{
	public override int Priority => 0;

	public override bool Draw(
		object                         holder,
		string                         name,
		ref object                     rawVal,
		List<EditorFieldBaseAttribute> attrs,
		int                            attrIdx)
	{
		if (rawVal is SyColor val)
		{
			if (SyProxyEditor.GeDrawColorField(name, false, ref val))
			{
				rawVal = val;
				return true;
			}
		}
		return false;
	}
}
}