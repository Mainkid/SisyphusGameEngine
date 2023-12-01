using System;
using System.Collections.Generic;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerEnum<T> : EditorDrawerBase<T> where T: Enum
{
	private readonly string[] _names;
	private readonly T[]      _values;

	public EditorDrawerEnum(SyProxyEditor editor) : base(editor)
	{
		_names  = Enum.GetNames(typeof(T));
		_values = (T[])Enum.GetValues(typeof(T));
	}

	protected override T DrawImpl(string name, T val)
	{
		var selected = 0;
		for (var i = 0; i < _values.Length; i++)
			if (EqualityComparer<T>.Default.Equals(_values[i], val))
			{
				selected = i;
				break;
			}

		selected = SyProxyEditor.GeDrawEnumField(name, _names, selected);
		return _values[selected];
	}
}
}