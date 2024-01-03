using System;
using System.Collections.Generic;
using System.Reflection;
using SyEngine.Ecs;
using SyEngine.Editor.Attributes;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerReflect<T> : EditorDrawerBase<T>
{
	private readonly List<FieldInfo> _fields = new List<FieldInfo>();

	private readonly Dictionary<FieldInfo, List<EditorFieldBaseAttribute>> _fieldToAttr;
	
	
	public EditorDrawerReflect(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs)
	{
		var fields = typeof(T).GetFields(BindingFlags.Instance | BindingFlags.Public);

		_fieldToAttr = new Dictionary<FieldInfo, List<EditorFieldBaseAttribute>>();
		foreach (var field in fields)
		{
			var isDrawable = true;
			
			foreach (var rawAttr in field.GetCustomAttributes())
			{
				if (rawAttr is HideAttribute)
				{
					isDrawable = false;
					break;
				}
                
				if (rawAttr.GetType().IsSubclassOf(typeof(EditorFieldBaseAttribute)))
				{
					if (!_fieldToAttr.TryGetValue(field, out var attrs))
					{
						attrs               = new List<EditorFieldBaseAttribute>();
						_fieldToAttr[field] = attrs;
					}

					var attr = (EditorFieldBaseAttribute)rawAttr;
					attr.Init(editor, ecs, typeof(T));

					attrs.Add(attr);
				}
			}
			if (isDrawable)
			{
				if (_fieldToAttr.TryGetValue(field, out var unsortedAttrs))
					unsortedAttrs.Sort((a, b)
						                   => b.Priority.CompareTo(a.Priority)
					);
				_fields.Add(field);
			}
			else
				_fieldToAttr.Remove(field);
		}
	}
    
	public override bool Draw(string name, ref T val)
	{
		object rawVal    = val;
		bool   isChanged = DrawRaw(name, ref rawVal);
		val = (T)rawVal;
		return isChanged;
	}

	public override bool DrawRaw(string name, ref object rawVal)
	{
		if (name != null)
		{
			SyProxyEditor.GeBeginGroup(name);
			
			SyProxyEditor.GeDrawText(name);
			SyProxyEditor.GeIndent(true);
		}

		var isChanged = false;
		
		if (rawVal == null)
		{
			SyProxyEditor.GeDrawText("[null]");
		}
		else
		{
			foreach (var field in _fields)
			{
				object fieldVal       = field.GetValue(rawVal);
				var    isFieldChanged = false;
				
				if (_fieldToAttr.TryGetValue(field, out var attrs))
				{
					isFieldChanged = attrs[0].Draw(
						rawVal,
						field.Name,
						ref fieldVal,
						attrs,
						0
					);
				}
				else
				{
					isFieldChanged = Editor.DrawField(field.Name, field.FieldType, ref fieldVal);
				}
				
				if (isFieldChanged)
				{
					field.SetValue(rawVal, fieldVal);
					isChanged = true;
				}
			}
		}

		if (name != null)
		{
			SyProxyEditor.GeIndent(false);
			
			SyProxyEditor.GeEndGroup();
		}
		
		return isChanged;
	}
}
}