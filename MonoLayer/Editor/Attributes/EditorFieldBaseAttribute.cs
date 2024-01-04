using System;
using System.Collections.Generic;
using SyEngine.Ecs;

namespace SyEngine.Editor.Attributes
{
public abstract class EditorFieldBaseAttribute : Attribute
{
	public void Init(SyProxyEditor editor, SyEcs ecs, Type holderType)
	{
		Editor     = editor;
		HolderType = holderType;
		InitImpl();
	}
    
	protected SyProxyEditor Editor     { get; private set; }
	protected SyEcs         Ecs        { get; private set; }
	protected Type          HolderType { get; private set; }

	public abstract int Priority { get; }

	protected virtual void InitImpl() { }

	public abstract bool Draw(
		object                         holder,
		string                         name,
		ref object                     rawVal,
		List<EditorFieldBaseAttribute> attrs,
		int                            attrIdx
	);

	protected bool ProcessDrawQueue(
		object                         holder,
		string                         name,
		ref object                     rawVal,
		List<EditorFieldBaseAttribute> attrs,
		int                            attrIdx)
	{
		return ++attrIdx < attrs.Count
			? attrs[attrIdx].Draw(holder, name, ref rawVal, attrs, attrIdx)
			: Editor.DrawField(name, rawVal.GetType(), ref rawVal);
	}
}
}