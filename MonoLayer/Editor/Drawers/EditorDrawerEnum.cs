﻿using System;
using System.Collections.Generic;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerEnum<T> : EditorDrawerBase<T> where T: Enum
{
	private readonly string[] _names;
	private readonly T[]      _values;

	public EditorDrawerEnum(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs)
	{
		_names  = Enum.GetNames(typeof(T));
		_values = (T[])Enum.GetValues(typeof(T));
	}

	public override bool Draw(string name, ref T val)
	{
		var selected = 0;
		for (var i = 0; i < _values.Length; i++)
			if (EqualityComparer<T>.Default.Equals(_values[i], val))
			{
				selected = i;
				break;
			}
		bool isChanged = SyProxyEditor.GeDrawEnumField(name, _names, ref selected);
		val = _values[selected];
		return isChanged;
	}
}
}