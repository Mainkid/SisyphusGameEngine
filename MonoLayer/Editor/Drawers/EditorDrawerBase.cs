using System;
using System.Collections.Generic;

namespace SyEngine.Editor.Drawers
{
public interface IEditorDrawer
{
	bool DrawRaw(string name, ref object rawVal);
}

public abstract class EditorDrawerBase<T> : IEditorDrawer
{
	protected readonly SyProxyEditor Editor;
	
	public EditorDrawerBase(SyProxyEditor editor)
	{
		Editor = editor;
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