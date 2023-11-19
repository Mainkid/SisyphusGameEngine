using System;
using System.Collections.Generic;

namespace SyEngine.Editor.Drawers
{
public interface IEditorDrawer
{
	bool DrawRaw(string name, ref object val);
}

public abstract class EditorDrawerBase<T> : IEditorDrawer
{
	protected SyProxyEditor Editor;
	
	public EditorDrawerBase(SyProxyEditor editor)
	{
		Editor = editor;
	}
	
	public virtual bool Draw(string name, ref T val)
	{
		var prevVal = val;
		var newVal  = DrawImpl(name, val);
		if (EqualityComparer<T>.Default.Equals(prevVal, newVal))
			return false;
		val = newVal;
		return true;
	}

	public virtual bool DrawRaw(string name, ref object val)
	{
		var prevVal = (T)val;
		var newVal  = DrawImpl(name, prevVal);
		if (EqualityComparer<T>.Default.Equals(prevVal, newVal))
			return false;
		val = newVal;
		return true;
	}

	protected virtual T DrawImpl(string name, T val)
	{
		throw new NotImplementedException(GetType().Name);
	}
}
}