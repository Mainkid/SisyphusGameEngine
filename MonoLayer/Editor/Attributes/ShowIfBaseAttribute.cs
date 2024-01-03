using System;
using System.Collections.Generic;
using System.Reflection;
using SyEngine.Logger;

namespace SyEngine.Editor.Attributes
{
[AttributeUsage(AttributeTargets.Field, AllowMultiple = true)]
public abstract class ShowIfBaseAttribute : EditorFieldBaseAttribute
{
	public sealed override int Priority => 50;

	protected FieldInfo FindField(string name)
	{
		return HolderType.GetField(
			name,
			BindingFlags.Instance |
			BindingFlags.Public |
			BindingFlags.NonPublic
		);
	}

	protected abstract bool CheckPredicate(object holder);


	public sealed override bool Draw(
		object                         holder,
		string                         name, 
		ref object                     rawVal, 
		List<EditorFieldBaseAttribute> attrs,
		int                            attrIdx)
	{
		if (!CheckPredicate(holder))
			return false;
		return ProcessDrawQueue(holder, name, ref rawVal, attrs, attrIdx);
	}
}
}