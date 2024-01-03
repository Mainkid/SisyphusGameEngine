using System;
using System.Collections.Generic;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public interface IEditorDrawer
{
	bool DrawRaw(string name, ref object rawVal);
}

public abstract class EditorDrawerBase<T> : IEditorDrawer
{
	protected readonly SyProxyEditor Editor;
	protected readonly SyEcs         Ecs;
	
	public EditorDrawerBase(SyProxyEditor editor, SyEcs ecs)
	{
		Editor = editor;
		Ecs    = ecs;
	}

	public abstract bool Draw(string name, ref T val);

	public virtual bool DrawRaw(string name, ref object rawVal)
	{
		var  val       = (T)rawVal;
		bool isChanged = Draw(name, ref val);
		if (isChanged)
			rawVal = val;
		return isChanged;
	}
}
}