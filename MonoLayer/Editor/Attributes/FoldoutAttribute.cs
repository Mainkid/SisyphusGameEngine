using System;
using System.Collections.Generic;

namespace SyEngine.Editor.Attributes
{
[AttributeUsage(AttributeTargets.Field)]
public class FoldoutAttribute : EditorFieldBaseAttribute
{
	public override int Priority => 30;

	public override bool Draw(
		object                         holder,
		string                         name,
		ref object                     rawVal,
		List<EditorFieldBaseAttribute> attrs,
		int                            attrIdx)
	{
		return SyProxyEditor.GeDrawFoldout(name) && ProcessDrawQueue(holder, name, ref rawVal, attrs, attrIdx);
	}
}
}